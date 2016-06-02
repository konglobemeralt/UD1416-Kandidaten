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
    vector<int> fileNamesPlayerNr; //innehåller i samma ordning som fileNames de olika spelar indexen
    vector<PlayerName> playerNames;

public:
    vector<PlayerInfo> playerInfo; //här finns all data sorterad nice, använd denna!!!!!!


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

                if (fileNamePointer[0] > 47 && fileNamePointer[0] < 58) { //är ett nummer
                    fileNames.push_back(fileNamePointer);
                }

            } while (FindNextFile(hFind, &data));
        }
        else 
        {
            string bajs = "Inte bra!";
        }

        FindClose(hFind);
        
    }

    void SortNamesAndFiles()
    {
        //int maxIndex = 0, minIndex = 0;
        for (int i = 0; i < fileNames.size(); i++)
        {
            int playerNr = 0;
            int startNameIndex = 0;

            if (fileNames[i][1] > 47 && fileNames[i][1] < 58)
            {
                playerNr += ((int)(fileNames[i][0]) - 48) * 10;
                playerNr += ((int)(fileNames[i][1]) - 48);
                startNameIndex = 2;
            }
            else 
            {
                startNameIndex = 1;
                playerNr += ((int)(fileNames[i][0]) - 48);
            }

            //spelar namnen!!!
            //TA REDA PÅ FÖRNAMNET
            int firstNameSize = 1; //gör inte while checken på första bokstaven för den är redan stor
            while (fileNames[i][firstNameSize+startNameIndex] < 'A' || fileNames[i][firstNameSize+startNameIndex] > 'Z') //det är en liten bokstav eller liknande
            {
                firstNameSize++;
            }
            firstNameSize += startNameIndex;

            char* playerFirstName = new char[firstNameSize];
            for (int f = startNameIndex; f < firstNameSize; f++)
            {
                playerFirstName[f - startNameIndex] = fileNames[i][f];
            }
            playerFirstName[firstNameSize-startNameIndex] = '\0';

            //TA REDA PÅ EFTERNAMNET
            int lastNameSize = 1; //gör inte while checken på första bokstaven för den är redan stor
            while (fileNames[i][firstNameSize + lastNameSize + startNameIndex] > 'a' && fileNames[i][firstNameSize + lastNameSize + startNameIndex] < 'z') //det är en liten bokstav eller liknande
            {
                lastNameSize++;
            }
            lastNameSize += startNameIndex;

            char* playerLastName = new char[lastNameSize];
            for (int f = startNameIndex; f < lastNameSize+startNameIndex; f++)
            {
                playerLastName[f - startNameIndex] = fileNames[i][firstNameSize+f-startNameIndex];
            }
            playerLastName[lastNameSize] = '\0';
            //**************************
            PlayerName playerName;
            playerName.firstName = playerFirstName;
            playerName.lastName = playerLastName;

            playerNames.push_back(playerName);
            fileNamesPlayerNr.push_back(playerNr);
        }
        //sortera!
        for (int i = 0; i < fileNames.size(); i++)
        {
            for (int y = 0; y < fileNames.size(); y++)
            {
                if (fileNamesPlayerNr[i] < fileNamesPlayerNr[y])
                {
                    int tempPlayerNr = fileNamesPlayerNr[i];
                    char* tempName = fileNames[i];
                    PlayerName tempPlayerName = playerNames[i];

                    fileNames[i] = fileNames[y];
                    fileNames[y] = tempName;
                    fileNamesPlayerNr[i] = fileNamesPlayerNr[y];
                    fileNamesPlayerNr[y] = tempPlayerNr;
                    playerNames[i] = playerNames[y];
                    playerNames[y] = tempPlayerName;
                }
            }
        }

        for (int i = 0; i < fileNames.size(); i++)
        {
            PlayerInfo pInfo;
            pInfo.fileName = fileNames[i];
            pInfo.playerNr = fileNamesPlayerNr[i];
            pInfo.playerName = playerNames[i];
            playerInfo.push_back(pInfo);
        }
        string bajs = "hej";
    }
};

#endif