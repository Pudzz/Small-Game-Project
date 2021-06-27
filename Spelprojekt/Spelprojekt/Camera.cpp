#include "Camera.h"

Camera::Camera(DirectX::XMMATRIX view, DirectX::XMMATRIX projection) : view(view), projection(projection)
{
	UpdatePlanes(projection, view);
}

Camera::Camera(float fovDegrees, size_t width, size_t height) : Object(ObjectLayer::None, nullptr, nullptr), width(width), height(height)
{
	this->aspect = static_cast<float>(width) / static_cast<float>(height);
	this->SetEnabled(false);
	SetFieldOfView(fovDegrees);
	UpdateView();
}

Camera::~Camera()
{

}

void Camera::SetFieldOfView(float fov)
{
	float aspect = static_cast<float>(width) / static_cast<float>(height);
	this->fovDegrees = fov;
	this->projection = DirectX::XMMatrixPerspectiveFovLH(fov * MathHelper::ToRadians, aspect, nearZ, farZ);
}

void Camera::UpdateView()
{
	Transform& transform = GetTransform();
	this->view = DirectX::XMMatrixLookToLH(
		transform.GetPosition(),
		transform.TransformDirection(Transform::default_forward),
		transform.TransformDirection(Transform::default_up)
	);

	//Update frustum planes
	UpdatePlanes(projection, view);
}

DirectX::XMMATRIX Camera::GetOrthographic() const
{
	return DirectX::XMMatrixOrthographicLH(static_cast<float>(width), static_cast<float>(height), nearZ, farZ);
}


bool Camera::IsBoundsInView(const AABB& aabb) const
{
	bool inViewResult = true;
	DirectX::XMFLOAT3 min, max, vmin, vmax;
	DirectX::XMStoreFloat3(&min, aabb.GetMin());
	DirectX::XMStoreFloat3(&max, aabb.GetMax());

	for (int i = 0; i < 6; i++)
	{
		DirectX::XMFLOAT4 plane = frustumPlanes[i];

		// X axis
		if (plane.x < 0)
		{
			vmin.x = min.x;
			vmax.x = max.x;
		}
		else
		{
			vmin.x = max.x;
			vmax.x = min.x;
		}

		// Y axis
		if (plane.y < 0)
		{
			vmin.y = min.y;
			vmax.y = max.y;
		}
		else
		{
			vmin.y = max.y;
			vmax.y = min.y;
		}

		// Z axis
		if (plane.z < 0)
		{
			vmin.z = min.z;
			vmax.z = max.z;
		}
		else
		{
			vmin.z = max.z;
			vmax.z = min.z;
		}

		float d1 = plane.x * vmin.x + plane.y * vmin.y + plane.z * vmin.z;
		if (d1 + plane.w < 0)
			return false;

		bool intersectsFrustum = false;

		if (intersectsFrustum)
		{
			float d2 = plane.x * vmax.x + plane.y * vmax.y + plane.z * vmax.z;
			if (d2 + plane.w <= 0)
				inViewResult = true;
		}
	}

	return inViewResult;
}

MathHelper::Ray Camera::ScreenPositionToWorldRay(const POINTS& point) const
{
	MathHelper::Ray ray;
	DirectX::FXMVECTOR v = { point.x, point.y, 0 };
	DirectX::XMVECTOR worldPos = DirectX::XMVector3Unproject(v, 0.0f, 0.0f, (float)width, (float)height, 0.0f, 1.0f, projection, view, DirectX::XMMatrixIdentity());
	ray.origin = GetTransform().GetPosition();
	ray.direction = DirectX::XMVectorSubtract(worldPos, ray.origin);
	ray.direction = DirectX::XMVector3Normalize(ray.direction);
	return ray;
}

void Camera::UpdatePlanes(DirectX::XMMATRIX projection, DirectX::XMMATRIX view)
{
	// http://www8.cs.umu.se/kurser/5DV051/HT12/lab/plane_extraction.pdf

	DirectX::XMMATRIX mds = DirectX::XMMatrixMultiply(view, projection);
	DirectX::XMFLOAT4X4 mat;
	XMStoreFloat4x4(&mat, mds);

	frustumPlanes[0] = { mat._14 - mat._11, mat._24 - mat._21, mat._34 - mat._31, mat._44 - mat._41 }; //right
	frustumPlanes[1] = { mat._14 + mat._11, mat._24 + mat._21, mat._34 + mat._31, mat._44 + mat._41 }; //left
	frustumPlanes[2] = { mat._14 + mat._12, mat._24 + mat._22, mat._34 + mat._32, mat._44 + mat._42 }; //bottom
	frustumPlanes[3] = { mat._14 - mat._12, mat._24 - mat._22, mat._34 - mat._32, mat._44 - mat._42 }; //top
	frustumPlanes[4] = { mat._13, mat._23, mat._33, mat._43 }; //near
	frustumPlanes[5] = { mat._14 - mat._13, mat._24 - mat._23, mat._34 - mat._33, mat._44 - mat._43 }; //far

	DirectX::XMFLOAT4 n;

	for (size_t i = 0; i < 6; i++)
	{
		n = frustumPlanes[i];
		float d = (float)sqrtf(n.x * n.x + n.y * n.y + n.z * n.z);

		frustumPlanes[i].x /= d;
		frustumPlanes[i].y /= d;
		frustumPlanes[i].z /= d;
		frustumPlanes[i].w /= d;
	}
}
