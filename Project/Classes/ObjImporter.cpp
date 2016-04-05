#include "ObjImporter.h"

ObjImporter::ObjImporter(wstring oFile, bool blendState){
	objFile = oFile;
	textureFile = L"";

	meshTriangles = 0;

	blend = blendState;
	
	box.isRendered = false;

	bboxPointsMax = { -float(INFINITE), -float(INFINITE), -float(INFINITE) };
	bboxPointsMin = { float(INFINITE), float(INFINITE), float(INFINITE) };

	specularColor_Power = { 0.0f, 0.0f, 0.0f, 1.0f };

	tempVertex.x = 0;
	tempVertex.y = 0;
	tempVertex.z = 0;
	tempVertex.u = 0;
	tempVertex.v = 0;
	vertex.clear();
	
}

ObjImporter::~ObjImporter(){

}

void ObjImporter::ObjLoad(){
	wifstream fileOpen(objFile.c_str());
	wifstream mtlOpen;
	wstringstream ss;
	wstring checkCharTemp[3];
	wstring checkBlock;
	wchar_t checkChar;

	wofstream myfile;

	if (fileOpen){
		while (fileOpen){
			checkChar = fileOpen.get(); // Get char one by one

			switch (checkChar){
			case 'v':
				checkChar = fileOpen.get();

				if (checkChar == ' '){ // Get vertex xyz (>> blockwise)
					float vertX, vertY, vertZ;
					fileOpen >> vertX >> vertY >> vertZ;
					vertexPos.push_back(XMFLOAT3(vertX, vertY, vertZ));
				}

				if (checkChar == 't'){ // Get vertex uv (>> blockwise)
					float vertU, vertV;
					fileOpen >> vertU >> vertV;
					textureCoord.push_back(XMFLOAT2(vertU, vertV));
				}

				if (checkChar == 'n'){ // Get vertex normal
					float vertNX, vertNY, vertNZ;
					fileOpen >> vertNX >> vertNY >> vertNZ;
					vertexNormal.push_back(XMFLOAT3(vertNX, vertNY, vertNZ));
					break;
				}
				
				break;
			case 'f':			
				checkChar = fileOpen.get();

				if (checkChar == ' '){
					for (int i = 0; i < 3; i++){
						fileOpen >> checkBlock;

						int k = 0;
						for (unsigned int b = 0; b <= checkBlock.size(); b++){
							if (checkBlock[b] != '/'){
								checkCharTemp[k].push_back(checkBlock[b]);
							}
							else{
								k++;
							}
						}

						tempVertex.x = vertexPos[stoi(checkCharTemp[0]) - 1].x;
						tempVertex.y = vertexPos[stoi(checkCharTemp[0]) - 1].y;
						tempVertex.z = vertexPos[stoi(checkCharTemp[0]) - 1].z;
						tempVertex.u = textureCoord[stoi(checkCharTemp[1]) - 1].x;
						tempVertex.v = textureCoord[stoi(checkCharTemp[1]) - 1].y;
						tempVertex.nx = vertexNormal[stoi(checkCharTemp[2]) - 1].x;
						tempVertex.ny = vertexNormal[stoi(checkCharTemp[2]) - 1].y;
						tempVertex.nz = vertexNormal[stoi(checkCharTemp[2]) - 1].z;
						
						//Get BoundingBox size
						if (tempVertex.x > bboxPointsMax.x)
							bboxPointsMax.x = tempVertex.x;

						if (tempVertex.x < bboxPointsMin.x)
							bboxPointsMin.x = tempVertex.x;

						if (tempVertex.y > bboxPointsMax.y)
							bboxPointsMax.y = tempVertex.y;

						if (tempVertex.y < bboxPointsMin.y)
							bboxPointsMin.y = tempVertex.y;

						if (tempVertex.z > bboxPointsMax.z)
							bboxPointsMax.z = tempVertex.z;

						if (tempVertex.z < bboxPointsMin.z)
							bboxPointsMin.z = tempVertex.z;

						// Add current vertex to struct-vector
						vertex.push_back(tempVertex);

						totalVertices++;
						checkCharTemp[0].clear();
						checkCharTemp[1].clear();
						checkCharTemp[2].clear();
					}
					meshTriangles++;
				}
				break;
			case '#':
				while (checkChar != '.'){
					checkChar = fileOpen.get();
				}
				break;
			case 'm':
				checkChar = fileOpen.get();
				if (checkChar == 't'){
					checkChar = fileOpen.get();
					if (checkChar == 'l'){
						while (checkChar != ' '){
							checkChar = fileOpen.get();
						}
						fileOpen >> checkBlock; // MTL-file
						mtlOpen.open(checkBlock);

						while (mtlOpen){
							checkChar = mtlOpen.get();
							switch (checkChar){
							case 'K':
								checkChar = mtlOpen.get();
								if (checkChar == 's'){
									mtlOpen >> specularColor_Power.x;
									mtlOpen >> specularColor_Power.y;
									mtlOpen >> specularColor_Power.z;
								}
								break;
							case 'N':
								checkChar = mtlOpen.get();
								if (checkChar == 's'){
									mtlOpen >> specularColor_Power.w;
								}
								break;
							case 'm':
								checkChar = mtlOpen.get();
								if (checkChar == 'a'){
									checkChar = mtlOpen.get();
									if (checkChar == 'p'){
										while (checkChar != ' '){
											checkChar = mtlOpen.get();
										}
										mtlOpen >> textureFile;
									}
								}
								break;
							}

							/*if (checkChar == mtlOpen.eof())
								mtlOpen.close();*/
						}
					}
				}				
				break;
			case 'g':
				break;
			case 'u':
				break;
			case 's':
				break;
			default: 
				break;
			}
			/*if (checkChar == fileOpen.eof())
				fileOpen.close();*/
		}

		XMFLOAT3 boxCenter = { (bboxPointsMax.x + bboxPointsMin.x) / 2, (bboxPointsMax.y + bboxPointsMin.y) / 2, (bboxPointsMax.z + bboxPointsMin.z) / 2 };

		if (abs(bboxPointsMin.x - boxCenter.x) > (bboxPointsMax.x - boxCenter.x))
			bboxPointsMax.x = abs(bboxPointsMin.x - boxCenter.x);
		else
			bboxPointsMax.x -= boxCenter.x;

		if (abs(bboxPointsMin.y - boxCenter.y) > (bboxPointsMax.y - boxCenter.y))
			bboxPointsMax.y = abs(bboxPointsMin.y - boxCenter.y);
		else
			bboxPointsMax.y -= boxCenter.y;

		if (abs(bboxPointsMin.z - boxCenter.z) > (bboxPointsMax.z - boxCenter.z))
			bboxPointsMax.z = abs(bboxPointsMin.z - boxCenter.z);
		else
			bboxPointsMax.z -= boxCenter.z;

		BoundingBox tempBox(boxCenter, bboxPointsMax);
		box.bBox = tempBox;

		myfile.open("text.txt");
		myfile << specularColor_Power.x << "\n" << specularColor_Power.y << "\n" << specularColor_Power.z << "\n" << specularColor_Power.w;
		myfile.close();
	}
}

size_t ObjImporter::GetSize(){
	return sizeof(tempVertex) * totalVertices;
}

vector<vertexData>* ObjImporter::GetData(){
	return &vertex;
}

wstring ObjImporter::ObjGetFileName(){
	return textureFile;
}