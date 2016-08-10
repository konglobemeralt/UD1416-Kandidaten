#pragma once
#ifndef INFO_RETRIEVER
#define INFO_RETRIEVER

#include <windows.h>
#include <tchar.h> 
#include <stdio.h>
#include <strsafe.h>
#include <string>
#include <vector>

#pragma comment(lib, "User32.lib")

using namespace std;

class InfoRetriever {
public:
    InfoRetriever() {}
    ~InfoRetriever()
    {
        for (unsigned int i = 0; i < playerInfo.size(); i++)
        {
            delete playerInfo[i].fileName;
            delete playerInfo[i].playerName.firstName;
            delete playerInfo[i].playerName.lastName;
        }
        playerInfo.clear();
		fileNames.clear();
		fileNameSizes.clear();
		playerOrderNr.clear();
		playerNr.clear();
		playerNames.clear();
    }
private:
    struct PlayerName
    {
        char* firstName;
        char* lastName;
    };

    struct PlayerInfo 
    {
        char* fileName;
        int playerNr;
        PlayerName playerName;
    };
    
    vector<char*> fileNames;
    vector<int> fileNameSizes;
    vector<int> playerOrderNr; //inneh�ller i samma ordning som fileNames de olika spelar indexen
    vector<int> playerNr;
    vector<PlayerName> playerNames;

public:
    vector<PlayerInfo> playerInfo; //h�r finns all data sorterad nice, anv�nd denna!!!!!!


    void Initialize()
    {
        FindFilesInDirectory();
        SortNamesAndFiles();
    }
    //vector<char*> playerNames;
    //vector<char*> fileNamesSorted;

private:

    void FindFilesInDirectory()
    {
        //char search_path[200];
        //sprintf(search_path, "%s*.*", folder.c_str()); //store path in search_path
        WIN32_FIND_DATA data;
        HANDLE hFind = ::FindFirstFile(L"Assets//Players/*.*", &data);

        if (hFind != INVALID_HANDLE_VALUE)
        {
            do 
            {
                int fileNameSize = lstrlen(data.cFileName) + 1;

                char* fileNamePointer = new char[fileNameSize];
                for (int i = 0; i < fileNameSize; i++)
                {
                    fileNamePointer[i] = char(data.cFileName[i]);
                }

                fileNamePointer[fileNameSize] = '\0';

                if (fileNamePointer[0] > 47 && fileNamePointer[0] < 58) { //�r ett nummer
                    fileNames.push_back(fileNamePointer);
                    fileNameSizes.push_back(fileNameSize);
                }
            } while (FindNextFile(hFind, &data));
        }
        else 
        {
            string errorstring = "Couldn't open this filemap!";
        }

        FindClose(hFind);
        
    }

    void SortNamesAndFiles()
    {
        //int maxIndex = 0, minIndex = 0;
        for (unsigned int i = 0; i < fileNames.size(); i++)
        {
            int playerOrder = 0;
            int startNameIndex = 0;

            if (fileNames[i][1] > 47 && fileNames[i][1] < 58)
            {
                playerOrder += ((int)(fileNames[i][0]) - 48) * 10;
                playerOrder += ((int)(fileNames[i][1]) - 48);
                startNameIndex = 2;
            }
            else 
            {
                startNameIndex = 1;
                playerOrder += ((int)(fileNames[i][0]) - 48);
            }

            //spelar namnen!!!
            //TA REDA P� F�RNAMNET
            int firstNameSize = 0; //g�r inte while checken p� f�rsta bokstaven f�r den �r redan stor
            while (fileNames[i][firstNameSize+startNameIndex] != '_') //det �r en liten bokstav eller liknande //fileNames[i][firstNameSize+startNameIndex] < 'A' || fileNames[i][firstNameSize+startNameIndex] > 'Z'
            {
                firstNameSize++;
            }
            //firstNameSize += startNameIndex;

            char* playerFirstName = new char[firstNameSize+1];
            for (int f = 0; f < firstNameSize; f++)
            {
                playerFirstName[f] = fileNames[i][f + startNameIndex];
            }
            playerFirstName[firstNameSize] = '\0';

            //TA REDA P� EFTERNAMNET
            int lastNameSize = 0; //g�r inte while checken p� f�rsta bokstaven f�r den �r redan stor
            while (fileNames[i][firstNameSize + lastNameSize + startNameIndex + 1] != '_') //+1 f�r att inte fastna p� f�rnamnets '_'
            {
                lastNameSize++;
            }
            //lastNameSize += startNameIndex;

            char* playerLastName = new char[lastNameSize+1];
            for (int f = 0; f < lastNameSize; f++)
            {
                playerLastName[f] = fileNames[i][firstNameSize+f+startNameIndex+1]; //+1 f�r att den hoppar �ver '_'
            }
            playerLastName[lastNameSize] = '\0';
            //**************************

            //SPELARNUMMER - enda kvar
            int playerNR = 0;
            int playerNRStartSpot = firstNameSize + lastNameSize + startNameIndex + 2;

            if (fileNames[i][playerNRStartSpot+1] > 47 && fileNames[i][playerNRStartSpot+1] < 58)
            {
                playerNR += ((int)(fileNames[i][playerNRStartSpot]) - 48) * 10;
                playerNR += ((int)(fileNames[i][playerNRStartSpot+1]) - 48);
            }
            else
            {
                playerNR += ((int)(fileNames[i][playerNRStartSpot]) - 48);
            }
            //*************

            PlayerName playerName;
            playerName.firstName = playerFirstName;
            playerName.lastName = playerLastName;

            playerNames.push_back(playerName);
            playerOrderNr.push_back(playerOrder);
            playerNr.push_back(playerNR);
        }
        //sortera!
        for (unsigned int i = 0; i < fileNames.size(); i++)
        {
            for (unsigned int y = 0; y < fileNames.size(); y++)
            {
                if (playerOrderNr[i] < playerOrderNr[y])
                {
                    int tempPlayerOrderNr = playerOrderNr[i];
                    char* tempName = fileNames[i];
                    PlayerName tempPlayerName = playerNames[i];
                    int tempPlayerNr = playerNr[i];

                    fileNames[i] = fileNames[y];
                    fileNames[y] = tempName;

                    playerOrderNr[i] = playerOrderNr[y];
                    playerOrderNr[y] = tempPlayerOrderNr;

                    playerNames[i] = playerNames[y];
                    playerNames[y] = tempPlayerName;

                    playerNr[i] = playerNr[y];
                    playerNr[y] = tempPlayerNr;
                }
            }
        }

		for (unsigned int i = 0; i < fileNames.size(); i++)
		{
			PlayerInfo pInfo;

			string filePathstring = "Players/";
			int filePathSize = filePathstring.length() + fileNameSizes[i];
            char* filePath = new char[filePathSize];
            int currIndex = 0;

            for (unsigned int y = 0; y < filePathstring.length(); y++)
            {
                filePath[currIndex] = filePathstring.at(y);
                currIndex++;
            }

            for (unsigned int y = 0; y < fileNameSizes[i]; y++)
            {
                filePath[currIndex] = fileNames[i][y];
                currIndex++;
            }

            pInfo.fileName = filePath;
            pInfo.playerNr = playerNr[i];
            pInfo.playerName = playerNames[i];
            playerInfo.push_back(pInfo);
        }

    }
};

#endif