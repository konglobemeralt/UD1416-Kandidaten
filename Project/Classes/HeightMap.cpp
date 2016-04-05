#include "HeightMap.h"

HeightMap::HeightMap(wstring textureFile1, wstring textureFile2, wstring textureFile3, wstring blendTexture, float terrainScale, float heightFactor){
	NumFaces = 0;
	NumVertices = 0;
	this->terrainScale = terrainScale;
	this->heightFactor = heightFactor;

	textureFile[0] = textureFile1;
	textureFile[1] = textureFile2;
	textureFile[2] = textureFile3;
	textureFile[3] = blendTexture;
}

void HeightMap::HeightMapLoad(char* filename){
	FILE *filePtr;							// Point to the current position in the file
	BITMAPFILEHEADER bitmapFileHeader;		// Structure which stores information about file
	BITMAPINFOHEADER bitmapInfoHeader;		// Structure which stores information about image
	int imageSize, index;
	unsigned char height;

	// Open the file
	fopen_s(&filePtr, filename, "rb");

	// Read bitmaps header
	fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);

	// Read the info header
	fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);

	// Get the width and height (width and length) of the image
	hminfo.terrainWidth = bitmapInfoHeader.biWidth;
	hminfo.terrainHeight = bitmapInfoHeader.biHeight;

	// Size of the image in bytes. the 3 represents RBG (byte, byte, byte) for each pixel
	imageSize = hminfo.terrainWidth * hminfo.terrainHeight * 3;

	// Initialize the array which stores the image data
	unsigned char* bitmapImage = new unsigned char[imageSize];

	// Set the file pointer to the beginning of the image data
	fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

	// Store image data in bitmapImage
	fread(bitmapImage, 1, imageSize, filePtr);

	// Close file
	fclose(filePtr);

	// Initialize the heightMap array (stores the vertices of our terrain)
	hminfo.heightMap = new XMFLOAT3[hminfo.terrainWidth * hminfo.terrainHeight];

	// We use a greyscale image, so all 3 rgb values are the same, but we only need one for the height
	// So we use this counter to skip the next two components in the image data (we read R, then skip BG)
	int k = 0;

	// We divide the height by this number to "water down" the terrains height, otherwise the terrain will
	// appear to be "spikey" and not so smooth.

	// Read the image data into our heightMap array
	for (int j = 0; j< hminfo.terrainHeight; j++)
	{
		for (int i = 0; i< hminfo.terrainWidth; i++)
		{
			height = bitmapImage[k];

			index = (hminfo.terrainHeight * j) + i;

			hminfo.heightMap[index].x = (float)i ;
			hminfo.heightMap[index].y = (float)height;
			hminfo.heightMap[index].z = (float)j;

			k += 3;
		}
	}

	delete[] bitmapImage;
	bitmapImage = 0;

	CreateGrid();
	CreateIndexList();
	CalculateNormals();
}

void HeightMap::CreateGrid(){
	cols = hminfo.terrainWidth;
	rows = hminfo.terrainHeight;

	//Create the grid
	NumVertices = rows * cols;
	NumFaces = (rows - 1)*(cols - 1) * 2;

	v.resize(NumVertices);

	for (DWORD i = 0; i < rows; ++i)
	{
		for (DWORD j = 0; j < cols; ++j)
		{
			v[i*cols + j].x = (hminfo.heightMap[i*cols + j].x * terrainScale);
			v[i*cols + j].y = (hminfo.heightMap[i*cols + j].y / heightFactor);
			v[i*cols + j].z = (hminfo.heightMap[i*cols + j].z * terrainScale);
			v[i*cols + j].nx = 0.0f;
			v[i*cols + j].ny = 1.0f;
			v[i*cols + j].nz = 0.0f;
		}
	}
}

void HeightMap::CreateIndexList(){
	indices.resize(NumFaces * 3);

	int k = 0;
	int texUIndex = 0;
	int texVIndex = 0;

	float scaleFactorU = 1.0f / float(rows);
	float scaleFactorV = 1.0f / float(cols);

	for (DWORD i = 0; i < rows - 1; i++)
	{
		for (DWORD j = 0; j < cols - 1; j++)
		{
			indices[k] = i*cols + j;		// Bottom left of quad
			v[i*cols + j].u = texUIndex + 0.0f;
			v[i*cols + j].v = texVIndex + 1.0f;
			v[i*cols + j].blendU = scaleFactorU * texUIndex; //x
			v[i*cols + j].blendV = (scaleFactorV * texVIndex) + 1; //y

			indices[k + 1] = i*cols + j + 1;		// Bottom right of quad
			v[i*cols + j + 1].u = texUIndex + 1.0f;
			v[i*cols + j + 1].v = texVIndex + 1.0f;
			v[i*cols + j + 1].blendU = (scaleFactorU * texUIndex) + 1;
			v[i*cols + j + 1].blendV = (scaleFactorV * texVIndex) + 1;

			indices[k + 2] = (i + 1)*cols + j;	// Top left of quad
			v[(i + 1)*cols + j].u = texUIndex + 0.0f;
			v[(i + 1)*cols + j].v = texVIndex + 0.0f;
			v[(i + 1)*cols + j].blendU = scaleFactorU * texUIndex;
			v[(i + 1)*cols + j].blendV = (scaleFactorV * texVIndex) + 1;

			indices[k + 3] = (i + 1)*cols + j;	// Top left of quad (second)
			indices[k + 4] = i*cols + j + 1;	// Bottom right of quad (second)

			indices[k + 5] = (i + 1)*cols + j + 1;	// Top right of quad
			v[(i + 1)*cols + j + 1].u = texUIndex + 1.0f;
			v[(i + 1)*cols + j + 1].v = texVIndex + 0.0f;
			v[(i + 1)*cols + j + 1].blendU = (scaleFactorU * texUIndex) + 1;
			v[(i + 1)*cols + j + 1].blendV = scaleFactorV * texVIndex;

			k += 6; // next quad

			texUIndex++;
		}
		texUIndex = 0;
		texVIndex++;
	}
}

void HeightMap::CalculateNormals(){
	//////////////////////Compute Normals///////////////////////////
	//Now we will compute the normals for each vertex using normal averaging
	vector<XMFLOAT3> tempNormal;

	//normalized and unnormalized normals
	XMFLOAT3 unnormalized = XMFLOAT3(0.0f, 0.0f, 0.0f);

	//Used to get vectors (sides) from the position of the verts
	float vecX, vecY, vecZ;

	//Two edges of our triangle
	XMVECTOR edge1 = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR edge2 = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

	//Compute face normals
	for (int i = 0; i < NumFaces; ++i)
	{
		//Get the vector describing one edge of our triangle (edge 0,2)
		vecX = v[indices[(i * 3)]].x - v[indices[(i * 3) + 2]].x;
		vecY = v[indices[(i * 3)]].y - v[indices[(i * 3) + 2]].y;
		vecZ = v[indices[(i * 3)]].z - v[indices[(i * 3) + 2]].z;
		edge1 = XMVectorSet(vecX, vecY, vecZ, 0.0f);	//Create our first edge

		//Get the vector describing another edge of our triangle (edge 2,1)
		vecX = v[indices[(i * 3) + 2]].x - v[indices[(i * 3) + 1]].x;
		vecY = v[indices[(i * 3) + 2]].y - v[indices[(i * 3) + 1]].y;
		vecZ = v[indices[(i * 3) + 2]].z - v[indices[(i * 3) + 1]].z;
		edge2 = XMVectorSet(vecX, vecY, vecZ, 0.0f);	//Create our second edge

		//Cross multiply the two edge vectors to get the un-normalized face normal
		XMStoreFloat3(&unnormalized, XMVector3Cross(edge1, edge2));
		tempNormal.push_back(unnormalized);			//Save unormalized normal (for normal averaging)
	}

	//Compute vertex normals (normal Averaging)
	XMVECTOR normalSum = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	int facesUsing = 0;
	float tX;
	float tY;
	float tZ;

	//Go through each vertex
	for (int i = 0; i < NumVertices; ++i)
	{
		//Check which triangles use this vertex
		for (int j = 0; j < NumFaces; ++j)
		{
			if (indices[j * 3] == i ||
				indices[(j * 3) + 1] == i ||
				indices[(j * 3) + 2] == i)
			{

				if (tempNormal[j].x < 0.0f)
					tempNormal[j].x *= -1.0f;
				if (tempNormal[j].y < 0.0f)
					tempNormal[j].y *= -1.0f;
				if (tempNormal[j].z < 0.0f)
					tempNormal[j].z *= -1.0f;

				tX = XMVectorGetX(normalSum) + tempNormal[j].x;
				tY = XMVectorGetY(normalSum) + tempNormal[j].y;
				tZ = XMVectorGetZ(normalSum) + tempNormal[j].z;

				normalSum = XMVectorSet(tX, tY, tZ, 0.0f);	//If a face is using the vertex, add the unormalized face normal to the normalSum
				facesUsing++;
			}
		}

		//Get the actual normal by dividing the normalSum by the number of faces sharing the vertex
		normalSum = normalSum / facesUsing;
		//Normalize the normalSum vector
		normalSum = XMVector3Normalize(normalSum);

		//Store the normal in our current vertex
		v[i].nx = XMVectorGetX(normalSum);
		v[i].ny = XMVectorGetY(normalSum);
		v[i].nz = XMVectorGetZ(normalSum);

		//Clear normalSum and facesUsing for next vertex
		normalSum = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		facesUsing = 0;
	}
}

void HeightMap::SetupBuffers(ID3D11Device* gDevice){
	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	/************************************New Stuff****************************************************/
	indexBufferDesc.ByteWidth = sizeof(DWORD)* NumFaces * 3;
	/*************************************************************************************************/
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA iinitData;

	/************************************New Stuff****************************************************/
	iinitData.pSysMem = &indices[0];
	/*************************************************************************************************/
	gDevice->CreateBuffer(&indexBufferDesc, &iinitData, &hmIndexBuffer);

	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	/************************************New Stuff****************************************************/
	vertexBufferDesc.ByteWidth = sizeof(vert) * NumVertices;
	/*************************************************************************************************/
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vertexBufferData;

	ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
	/************************************New Stuff****************************************************/
	vertexBufferData.pSysMem = &v[0];
	/*************************************************************************************************/
	gDevice->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &hmVertexBuffer);

	ID3D11ShaderResourceView* texture;
	for (int i = 0; i < 4; i++){
		CreateWICTextureFromFile(gDevice, textureFile[i].c_str(), nullptr, &texture);
		textureSRV.push_back(texture);
	}
}

void HeightMap::DrawHM(ID3D11DeviceContext* gDeviceContext, ID3D11VertexShader* HMVertexShader, ID3D11PixelShader* HMPixelShader, ID3D11ShaderResourceView* lightDepthMap){
	UINT32 vertexSize = sizeof(vert);
	UINT32 offset = 0;

	textureSRV.push_back(lightDepthMap);

	gDeviceContext->VSSetShader(HMVertexShader, nullptr, 0);
	gDeviceContext->GSSetShader(nullptr, nullptr, 0);
	gDeviceContext->PSSetShader(HMPixelShader, nullptr, 0);

	gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	gDeviceContext->PSSetShaderResources(0, 5, textureSRV.data());
	gDeviceContext->IASetVertexBuffers(0, 1, &hmVertexBuffer, &vertexSize, &offset);
	gDeviceContext->IASetIndexBuffer(hmIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	gDeviceContext->DrawIndexed(NumFaces*3, 0, 0);

	textureSRV.pop_back();
}

float HeightMap::getTerrainHeight(float camX, float camZ){
	if (camX > FLT_EPSILON && camX < (float(cols - 1) * terrainScale) && camZ > FLT_EPSILON && camZ < (float(rows - 1) * terrainScale)){
		float c = camX / terrainScale;
		float d = camZ / terrainScale;

		int rowIndex = int(floorf(d));
		int colIndex = int(floorf(c));

		float s = c - float(colIndex);
		float t = d - float(rowIndex);

		float A = v[(rowIndex * cols) + colIndex].y;
		float B = v[(rowIndex * cols) + colIndex + 1].y;
		float C = v[((rowIndex + 1) * cols) + colIndex].y;
		float D = v[((rowIndex + 1) * cols) + colIndex + 1].y;

		if (s + t <= 1.0f)
		{
			float uy = B - A;
			float vy = C - A;
			return 3.0f + A + s*uy + t*vy;
		}
		else // lower triangle DCB.
		{
			float uy = C - D;
			float vy = B - D;
			return 3.0f + D + (1.0f - s)*uy + (1.0f - t)*vy;
		}
	}
	else
		return 3.0f;
}

HeightMap::~HeightMap(){
	hmVertexBuffer->Release();
	hmIndexBuffer->Release();

	for (unsigned int i = 0; i < textureSRV.size(); i++){
		textureSRV[i]->Release();
	}
}