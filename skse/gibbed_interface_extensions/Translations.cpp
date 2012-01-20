#include <string>
#include <Windows.h>

#include "skse/GameAPI.h"
#include "skse/SafeWrite.h"
#include "skse/Utilities.h"
#include "common/IDirectoryIterator.h"
#include "common/IFileStream.h"


bool GenerateMergedTranslateFile()
{
	IFileStream mergedFile = IFileStream();
	IFileStream inputFile = IFileStream();

	std::string	runtimeDirectory = GetRuntimeDirectory();

	if(runtimeDirectory.empty())
		return false;
	
	// Language not set
	if (*g_gameLanguage == NULL)
		return false;


	// Initialize the merged file with default translates
	mergedFile.Create("Data\\Interface\\Translate_MERGED.txt");

	std::string defaultTranslateFilePath = runtimeDirectory + "Data\\Interface\\Translate_" + *g_gameLanguage + ".txt";
	if (inputFile.Open(defaultTranslateFilePath.c_str())) {
		IFileStream::CopyStreams(&mergedFile, &inputFile);
		inputFile.Close();
		_DMESSAGE("Translations: merged %s", defaultTranslateFilePath.c_str());

	// Couldn't copy the default file, so bail out and use the original location instead
	} else {
		mergedFile.Close();
		return false;
	}

	std::string translateDirectory = runtimeDirectory + "Data\\Interface\\Translations";

	bool found = false;

	for (IDirectoryIterator iter(translateDirectory.c_str(), "*"); !iter.Done(); iter.Next()) {
		
		WIN32_FIND_DATA* result = iter.Get();

		if (result) {
			DWORD attr = result->dwFileAttributes;
			if (attr & FILE_ATTRIBUTE_DIRECTORY) {
				std::string	modPath = iter.GetFullPath();

				std::string modTranslateFilePath = iter.GetFullPath() + "\\" + *g_gameLanguage + ".txt";
				if (inputFile.Open(modTranslateFilePath.c_str())) {
					found = true;

					// Add extra newline
					mergedFile.Write16(0x000D);
					mergedFile.Write16(0x000A);

					// Skip BOM
					inputFile.Skip(2);

					IFileStream::CopySubStreams(&mergedFile, &inputFile, inputFile.GetRemain());
					inputFile.Close();
					_MESSAGE("Translations: merged %s", modTranslateFilePath.c_str());
				}
			}
		}
	}

	mergedFile.Close();

	// No mod added translations added => no merged file
	if (!found) {
		_DMESSAGE("no additional translations detected, using default file");
		return false;
	}

	return true;
}


int __cdecl MyGetTranslateFilePath(char* translateFilePath, size_t sizeInBytes, const char* format, ...)
{
	char defaultTranslateFilePath[0x40];

	va_list args;
	va_start(args, format);
	int len = vsprintf_s(defaultTranslateFilePath, sizeInBytes, format, args);
	va_end(args);

	// Attempt to create merged translate file, otherwise fall-back to default
	if (GenerateMergedTranslateFile())
		strcpy(translateFilePath, "Interface\\Translate_MERGED.txt");
	else
		strcpy(translateFilePath, defaultTranslateFilePath);

	return len;
}


bool PatchTranslations()
{
	UInt32 getTranslateFilePathCall = 0x00BF21F6;

	unsigned char original[] = { 0xE8, 0xD5, 0x75, 0x84, 0xFF };
	if (memcmp((void *)getTranslateFilePathCall, original, sizeof(original)) != 0)
		return false;

	WriteRelCall(getTranslateFilePathCall, (UInt32)&MyGetTranslateFilePath);

	return true;
}