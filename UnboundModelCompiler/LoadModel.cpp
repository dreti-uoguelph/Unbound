#include"stdafx.h"
#include"LoadModel.h"
#include<assimp\Importer.hpp>
#include<assimp\scene.h>
#include<assimp\postprocess.h>

Model loadModel(string filePath, int tnum, int* numErrors)
{
	Model toReturn = {};
	cout << tnum << ">	" << " loading file: " << filePath << endl;

	Assimp::Importer importer;
	auto scene = importer.ReadFile(filePath, aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_GenSmoothNormals | aiProcess_Triangulate | aiProcess_FlipWindingOrder | aiProcess_ConvertToLeftHanded | aiProcess_FlipUVs | aiProcess_GenUVCoords);
	scene = importer.ApplyPostProcessing(aiProcess_CalcTangentSpace);

	if (!scene || scene->mNumMeshes == 0)
	{
		cout << tnum << ">	Could not load model: " << filePath << endl;
		numErrors++;
		return Model();
	}

	auto numModels = scene->mNumMeshes;

	for (uint32_t i = 0; i < numModels; i++)
	{
		toReturn.numVertices += scene->mMeshes[i]->mNumVertices;
		toReturn.numIndices += scene->mMeshes[i]->mNumFaces * 3;
	}

	toReturn.vertices = reinterpret_cast<Vertex*>(malloc(toReturn.numVertices * sizeof(Vertex)));
	toReturn.indices = reinterpret_cast<uint32_t*>(malloc(toReturn.numIndices * sizeof(uint32_t)));

	uint32_t vertexOffset = 0;
	uint32_t indexOffset = 0;

	for (uint32_t i = 0; i < numModels; i++)
	{
		uint32_t modelVertices = scene->mMeshes[i]->mNumVertices;
		uint32_t modelFaces = scene->mMeshes[i]->mNumFaces;

		auto verts = scene->mMeshes[i]->mVertices;
		auto norms = scene->mMeshes[i]->mNormals;
		auto tans = scene->mMeshes[i]->mTangents;
		auto uvs = scene->mMeshes[i]->mTextureCoords;
		auto colors = scene->mMeshes[i]->mColors;

		auto faces = scene->mMeshes[i]->mFaces;

		uint32_t l = vertexOffset;
		for (uint32_t j = 0; j < modelVertices; j++)
		{
			toReturn.vertices[l].position = glm::vec3(verts[j].x, verts[j].y, verts[j].z);
			toReturn.vertices[l].normal = glm::vec3(norms[j].x, norms[j].y, norms[j].z);

			if (scene->mMeshes[i]->HasTangentsAndBitangents())
			{
				toReturn.vertices[l].tangent = glm::vec3(tans[j].x, tans[j].y, tans[j].z);
			}
			else
			{
				toReturn.vertices[l].tangent = glm::vec3(0.0f, 0.0f, 0.0f);
			}

			if (scene->mMeshes[i]->HasVertexColors(0))
			{
				toReturn.vertices[l].color = glm::vec3(colors[j][0].r, colors[j][0].g, colors[j][0].b);
			}
			else
			{
				toReturn.vertices[l].color = glm::vec3(1.0f, 1.0f, 1.0f);
			}

			if (scene->mMeshes[i]->HasTextureCoords(0))
			{
				toReturn.vertices[l].uv = glm::vec2(uvs[0][j].x, uvs[0][j].y);
			}
			else
			{
				toReturn.vertices[l].uv = glm::vec2(0.0f, 0.0f);
			}

			if (scene->mMeshes[i]->HasTextureCoords(1))
			{
				toReturn.vertices[l].uv2 = glm::vec2(uvs[j][1].x, uvs[j][1].y);
			}

			l++;
		}

		uint32_t j = indexOffset;
		for (uint32_t k = 0; k < modelFaces; k++)
		{
			memcpy(&toReturn.indices[j], faces[k].mIndices, sizeof(uint32_t) * 3);
			j += 3;
		}

		vertexOffset += modelVertices;
		indexOffset += modelFaces * 3;
	}

	return toReturn;
}