#pragma once

#include <assimp/Importer.hpp>
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <vector>
#include "Mesh.h"
#include "Logger.h"
#include "Texture.h"
#include "MathHelper.h"
#include "ConstantBuffers.h"
#include "Shader.h"
#include "Material.h"

// Namespace for controlling the assimpHandler
namespace AssimpHandler
{
	// Struct to save all mesh and material data from the FBX file
	struct AssimpData
	{
		Mesh* mesh;
		Material* material;

		AssimpData() : mesh(nullptr), material(nullptr) {}
	};

	inline Mesh* loadMesh(const aiScene* scene, ID3D11Device* device)
	{
		Mesh* newMesh = nullptr;

		std::vector<MeshVertex> vertices;
		std::vector<unsigned int> indices;
		std::vector<DirectX::XMFLOAT2> uvs;
		std::vector<DirectX::XMFLOAT3> normals;

		int nrOfVertices;

		// For every mesh in the scene, at the moment, only supports one mesh per file
		for (int i = 0; i < 1; i++)
		{
			// Current mesh and amount of vertices
			aiMesh* meshes = scene->mMeshes[i];
			nrOfVertices = meshes->mNumVertices;

			// For every vertex in the current mesh
			for (int j = 0; j < nrOfVertices; j++)
			{
				// Stores the information from the current mesh to a temp vertex struct
				MeshVertex temp;
				// Positions
				temp.position.x = meshes->mVertices[j].x; temp.position.y = meshes->mVertices[j].y; temp.position.z = meshes->mVertices[j].z;
				// Normals
				temp.normal.x = meshes->mNormals[j].x; temp.normal.y = meshes->mNormals[j].y; temp.normal.z = meshes->mNormals[j].z;
				// UVs
				temp.uv.x = meshes->mTextureCoords[0][j].x; temp.uv.y = meshes->mTextureCoords[0][j].y;
				// Tangents
				temp.tangent.x = meshes->mTangents[j].x; temp.tangent.y = meshes->mTangents[j].y; temp.tangent.z = meshes->mTangents[j].z;

				vertices.push_back(temp);
			}

			// For every face, stores the indices
			for (unsigned int faceIndex = 0; faceIndex < meshes->mNumFaces; faceIndex++)
			{
				indices.push_back(meshes->mFaces[faceIndex].mIndices[0u]);
				indices.push_back(meshes->mFaces[faceIndex].mIndices[1u]);
				indices.push_back(meshes->mFaces[faceIndex].mIndices[2u]);
			}
		}

		// If the scene contains more than 0 bones, then we want to collect the bone information for animations
		Skeleton* tempSkeleton = new Skeleton();
		if (scene->mMeshes[0]->mNumBones > 0)
		{
			// Store the global mesh inverse transform for the skeleton, if the mesh is stored correctly, this will almost certain be indentity
			tempSkeleton->SetGlobalMeshInverseTransform(DirectX::XMMATRIX(&scene->mRootNode->mTransformation.Inverse().a1));

			// For every bone in the mesh
			for (unsigned int i = 0; i < scene->mMeshes[0]->mNumBones; i++)
			{
				unsigned int boneIndex = 0;
				std::string boneName(scene->mMeshes[0]->mBones[i]->mName.data);

				// Checks if the skeleton already contains the current bone, otherwise stores it in a map
				if (tempSkeleton->boneMapping.find(boneName) == tempSkeleton->boneMapping.end())
				{
					boneIndex = tempSkeleton->GetNumberOfBones();
					tempSkeleton->AddBoneAmount();
					tempSkeleton->boneMapping[boneName] = boneIndex;
					Joint bone;
					
					// Stores the inverse bind pose of the skeleton
					bone.SetInverseBindPose(DirectX::XMMATRIX(&scene->mMeshes[0]->mBones[i]->mOffsetMatrix.a1));
					bone.SetBoneName(scene->mMeshes[0]->mBones[i]->mName.data);
					tempSkeleton->AddNewBone(bone);
				}
				else
				{
					boneIndex = tempSkeleton->boneMapping[boneName];
				}

				// For every weight in the bone
				for (unsigned int j = 0; j < scene->mMeshes[0]->mBones[i]->mNumWeights; j++)
				{
					// Get the ID of which vertex this weight is affecting
					unsigned int vertexId = scene->mMeshes[0]->mBones[i]->mWeights[j].mVertexId;
					// Weight amount
					float weight = scene->mMeshes[0]->mBones[i]->mWeights[j].mWeight;

					// If this vertice weight hasnt been changed already from 0, change it
					// Only supports 1 weight per vertex
					if (vertices[vertexId].weights == 0.0f)
					{
						vertices[vertexId].IDS = boneIndex;
						vertices[vertexId].weights = weight;
					}
				}
			}
		}

		// Creates a new mesh with our own structure, storing the vertices, indices and the skeleton
		// If there was no skeleton to save, it will save a nullptr
		newMesh = MeshCreator::CreateMesh(vertices, indices, device);
		newMesh->skeleton = tempSkeleton;

		return newMesh;
	}

	inline Texture* loadTextureFromFbx(DX11Handler& dx11, aiString path)
	{
		Texture* texture = nullptr;
		std::string fileName = "Textures/";

		// Convert the filepath to a regular string and crop it so it will only be the name of the texture
		std::string nameString = path.C_Str();
		std::size_t pos = nameString.find_last_of("/\\");

		// Saves the name of the texture to our own filepath in our program, "Textures"
		fileName += nameString.substr(pos + 1);

		// Create a new texture and then return it
		texture = Texture::CreateTexture(fileName, dx11);
		return texture;
	}

	inline MaterialData getMaterialFromFbx(const aiScene* scene)
	{
		// Get all the materials in the scene
		aiMaterial** materialArray = scene->mMaterials;
		aiColor4D aiSpecular, aiDiffuse, aiAmbient;
		float aiShininess;

		// Get the attributes of our choosing and save it
		aiGetMaterialColor(materialArray[0], AI_MATKEY_COLOR_SPECULAR, &aiSpecular);
		aiGetMaterialColor(materialArray[0], AI_MATKEY_COLOR_DIFFUSE, &aiDiffuse);
		aiGetMaterialColor(materialArray[0], AI_MATKEY_COLOR_AMBIENT, &aiAmbient);
		aiGetMaterialFloat(materialArray[0], AI_MATKEY_SHININESS, &aiShininess);

		// Convert the attributes from Assimps structures to DirectX XMFloat4
		DirectX::XMFLOAT4 specular, diffuse, ambient;
		specular.x = aiSpecular.r; specular.y = aiSpecular.g; specular.z = aiSpecular.b; specular.w = aiSpecular.a;
		diffuse.x = aiDiffuse.r; diffuse.y = aiDiffuse.g; diffuse.z = aiDiffuse.b; diffuse.w = aiDiffuse.a;
		ambient.x = aiAmbient.r; ambient.y = aiAmbient.g; ambient.z = aiAmbient.b; ambient.w = aiAmbient.a;

		// Create a new material of our own structure and return it
		MaterialData material;
		material.diffuse = diffuse;
		material.ambient = ambient;
		material.specular = specular;
		material.shininess = aiShininess;
		return material;
	}

	inline const aiNodeAnim* checkNodeAnim(const aiAnimation* animation, const std::string nodeName)
	{
		// For every channel/curve in the current animation
		for (unsigned int i = 0; i < animation->mNumChannels; i++)
		{
			// Checks if the node responding to this channel, is the same node we are on in the main loop
			const aiNodeAnim* nodeAnim = animation->mChannels[i];
			if (std::string(nodeAnim->mNodeName.data) == nodeName)
			{
				// returns the node
				return nodeAnim;
			}
		}
		// If the node is not in this animation, return null
		return NULL;
	}

	inline void getPositionAtKeyframe(aiVector3D& position, unsigned int keyframe, const aiNodeAnim* nodeAnim)
	{
		// Check if there are more than 1 key of translation, otherwise return the translation at keyframe 0
		if (nodeAnim->mNumPositionKeys == 1)
		{
			position = nodeAnim->mPositionKeys[0].mValue;
			return;
		}

		// Get the position at the current keyframe
		const aiVector3D& pos = nodeAnim->mPositionKeys[keyframe].mValue;
		position = pos;

	}

	inline void getRotationAtKeyframe(aiQuaternion& rotation, unsigned int keyframe, const aiNodeAnim* nodeAnim)
	{
		// Check if there are more than 1 key of rotation, otherwise return the rotation at keyframe 0
		if (nodeAnim->mNumRotationKeys == 1)
		{
			rotation = nodeAnim->mRotationKeys[0].mValue;
			return;
		}

		// Get the rotation at the current keyframe and normalize it
		aiQuaternion& rot = nodeAnim->mRotationKeys[keyframe].mValue;
		rot.Normalize();
		rotation = rot;
	}

	inline void getScaleAtKeyframe(aiVector3D& scale, unsigned int keyframe, const aiNodeAnim* nodeAnim)
	{
		// Check if there are more than 1 key of scaling, otherwise return the scaling at keyframe 0
		if (nodeAnim->mNumScalingKeys == 1)
		{
			scale = nodeAnim->mScalingKeys[0].mValue;
			return;
		}

		// Get the scaling at the current keyframe
		aiVector3D& scaling = nodeAnim->mScalingKeys[keyframe].mValue;
		scale = scaling;
	}

	inline void ReadSceneHierarchy(unsigned int keyframe, const aiScene* scene, aiNode* node, DirectX::XMMATRIX parentTransform, Animation* animation, Skeleton* skeleton)
	{
		// Name of the current node
		std::string nodeName = node->mName.data;

		// First animation in the scene
		const aiAnimation* assimpAnimation = scene->mAnimations[0];

		// The nodes transformation relative to its parent
		// If this node is affected in the animation, it will be overwritten later
		DirectX::XMMATRIX nodeTransformation = DirectX::XMMATRIX(&node->mTransformation.a1);

		// Check if the node is affected by the animation
		const aiNodeAnim* nodeAnim = checkNodeAnim(assimpAnimation, nodeName);

		// If its not null, the nodes transformation will be overwritten here
		if (nodeAnim)
		{
			aiVector3D scaling, translation;
			aiQuaternion rotation;

			// Get the position, rotation and scaling at the current keyframe
			getPositionAtKeyframe(translation, keyframe, nodeAnim);
			getRotationAtKeyframe(rotation, keyframe, nodeAnim);
			getScaleAtKeyframe(scaling, keyframe, nodeAnim);

			// Convert the data from Assimps structure to DirectX XMVector
			DirectX::XMVECTOR scale = DirectX::XMVectorSet(scaling.x, scaling.y, scaling.z, 1.0f);
			DirectX::XMVECTOR trans = DirectX::XMVectorSet(translation.x, translation.y, translation.z, 1.0f);
			DirectX::XMVECTOR rotate = DirectX::XMVectorSet(rotation.x, rotation.y, rotation.z, 1.0f);
			DirectX::XMVECTOR origin = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

			// Create a transformation matrix and transpose it, for multiplication purposes
			nodeTransformation = DirectX::XMMatrixAffineTransformation(scale, origin, rotate, trans);
			nodeTransformation = DirectX::XMMatrixTranspose(nodeTransformation);
		}
		
		// Multiply the transformation with its parent
		DirectX::XMMATRIX globalTransform = parentTransform * nodeTransformation;

		// If the current node is also a bone in the skeleton
		if (skeleton->boneMapping.find(nodeName) != skeleton->boneMapping.end())
		{
			unsigned int boneIndex = skeleton->boneMapping[nodeName];

			// Save these to the animation struct, just for future reference
			animation->GetBone(boneIndex).SetGlobalTransform(globalTransform);
			animation->GetBone(boneIndex).SetInverseBindPose(skeleton->GetBone(boneIndex).GetInverseBindPose());

			// Combine the transformation with the skeletons globalmeshInverse and the bones inverse bind pose
			DirectX::XMMATRIX final = skeleton->GetGlobalMeshInverse() * globalTransform * skeleton->GetBone(boneIndex).GetInverseBindPose();

			// Save the transformation to the corresponding bone in the animation class
			animation->GetBone(boneIndex).SetFinalTransformation(final);
		}

		// Repeat the function recursively for each child
		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			ReadSceneHierarchy(keyframe, scene, node->mChildren[i], globalTransform, animation, skeleton);
		}

	}

	inline void saveAnimationData(const aiScene* scene, Skeleton* skeleton, std::string animName)
	{
		DirectX::XMMATRIX identityMatrix = DirectX::XMMatrixIdentity();

		// If there is no animations in this scene
		if (!scene->mAnimations[0])
		{
			Logger::Write("There was no animations in: " + animName);
		}

		else
		{
			// Length of the animation in ticks/frames
			float animationLength = (float)scene->mAnimations[0]->mDuration;

			// Store the needed animation data to our own animation class
			// Amount of bones is stored from the skeleton passed in the function
			Animation* newAnimation = new Animation();
			newAnimation->SetLength((unsigned int)animationLength);
			newAnimation->SetBoneAmount(skeleton->GetNumberOfBones());
			newAnimation->GetBoneVector().resize(newAnimation->GetNumberOfBones());
			newAnimation->SetName(animName);

			// Read the scenes hierarchy once, for every keyframe
			for (unsigned int i = 0; i < animationLength; i++)
			{
				ReadSceneHierarchy(i, scene, scene->mRootNode, identityMatrix, newAnimation, skeleton);
			}

			// Saves the animation to the skeleton
			skeleton->animations.push_back(newAnimation);
		}
		
	}

	inline AssimpData* loadFbxObject(const char* filepath, DX11Handler& dx11, Shader* shader, ID3D11SamplerState* sampler = nullptr)
	{
		if (sampler == nullptr)
			sampler = dx11.GetDefaultSampler();

		// Open the scene from the file
		Assimp::Importer imp;

		// Read the FBX file, the aiProcessPreset flags make sure that the scene is converted to left hand from Maya
		// and makes sure that the mesh is triangulated 
		const aiScene* scene = imp.ReadFile(filepath, aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_ConvertToLeftHanded);
		AssimpData* object = new AssimpData();

		// If there is something wrong with the scene
		if (scene == nullptr)
		{
			Logger::Write("Couldnt open file with Assimp");
		}

		else
		{
			Texture* normalMap = nullptr;
			Texture* texture = nullptr;
			aiString path;

			// Create a new object with the new mesh
			// Get the mesh from the file
			object->mesh = loadMesh(scene, dx11.GetDevice());
			object->material = new Material(shader, dx11);

			// Get the materials from the file
			MaterialData temp = getMaterialFromFbx(scene);
			object->material->SetMaterialData(temp);

			// Check if the file contains a diffuseTexture
			if (scene->mMaterials[0]->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS)
			{
				// Load the diffuseTexture and apply it to the object
				texture = loadTextureFromFbx(dx11, path);
				object->material->GetMaterialData().hasAlbedoTexture = true;
			}

			// The app assumes there is a texture to every object, so if there is no texture in the file,
			// add a simple grey texture to it
			else
			{
				texture = Texture::CreateTexture("Textures/greyTexture.png", dx11);
			}

			// Check if the file contains a normalMap
			if (scene->mMaterials[0]->GetTexture(aiTextureType_NORMALS, 0, &path) == AI_SUCCESS)
			{
				// Load the normalMap and apply it to the object
				normalMap = loadTextureFromFbx(dx11, path);
				object->material->SetTexture(NORMAL_MATERIAL_TYPE, normalMap, SHADER_BIND_TYPE::PIXEL);
				object->material->SetSampler(NORMAL_MATERIAL_TYPE, sampler, SHADER_BIND_TYPE::PIXEL);
				object->material->GetMaterialData().hasNormalTexture = true;
				
			}

			// Saves the albedo texture for the object
			object->material->SetTexture(ALBEDO_MATERIAL_TYPE, texture, SHADER_BIND_TYPE::PIXEL);
		}
		return object;
	}
}
