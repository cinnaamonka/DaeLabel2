#include "Helpers.h"

namespace GameEngine
{
	void GetVerticesFromJsonFile(std::string fileName, std::vector<std::vector<glm::vec3>>& m_Vertices, std::vector<std::vector<glm::vec3>>& m_VerticesSecondType)
	{
		std::filesystem::path currentPath = std::filesystem::current_path();
		std::filesystem::path parentPath = currentPath.parent_path();

		std::filesystem::path dataPath = parentPath / "Data";

		std::filesystem::path filePath = dataPath / fileName;

		std::ifstream inputFile(filePath);

		if (!inputFile.is_open())
		{
			std::cerr << "Failed to open " << fileName << std::endl;
			return;
		}

		nlohmann::json jsonDoc;

		try
		{
			inputFile >> jsonDoc;

			std::vector<std::vector<glm::vec3>> allBlocks;

			for (const auto& blockData : jsonDoc["level"])
			{
				glm::vec3 position;
				position.x = blockData["position"]["left"];
				position.y = blockData["position"]["top"];
				std::string type = blockData["type"];

				const int size = 20;

				std::vector<glm::vec3> block{};
				block.push_back(position);
				block.push_back({ position.x + size, position.y,0 });
				block.push_back({ position.x, position.y + size,0 });
				block.push_back({ position.x + size, position.y + size,0 });

				if (type == "ice_block")
				{
					m_Vertices.push_back(block);
				}
				else
				{
					m_VerticesSecondType.push_back(block);
				}

				m_Vertices.push_back(block);
			}
		}
		catch (const nlohmann::json::parse_error& e)
		{
			std::cerr << "JSON Parsing error: " << e.what() << std::endl;
			return;
		}
	}

	bool IsOverlapping(const Rect& r1, const Rect& r2)
	{
		// If one rectangle is on left side of the other
		if ((r1.left + r1.width) < r2.left || (r2.left + r2.width) < r1.left)
		{
			return false;
		}

		// If one rectangle is under the other
		if (r1.bottom > (r2.bottom + r2.height) || r2.bottom > (r1.bottom + r1.height))
		{
			return false;
		}

		return true;
	}

	bool IsPointOnLineSegment(const glm::vec2& p, const glm::vec2& a, const glm::vec2& b)
	{
		glm::vec3 ap{ a.x - p.x, a.y - p.y, 0 }, bp{ b.x - p.x, b.y - p.y, 0 };
		// If not on same line, return false
		if (glm::length(glm::cross(ap, bp)) > 0.001f)
		{
			return false;
		}

		// Both vectors must point in opposite directions if p is between a and b
		if (glm::dot(ap, bp) > 0)
		{
			return false;
		}

		return true;
	}

	bool IntersectLineSegments(const glm::vec2& p1, const glm::vec2& p2, const glm::vec2& q1, const glm::vec2& q2, float& outLambda1, float& outLambda2, float epsilon)
	{
		bool intersecting{ false };

		// Calculate vectors from points
		glm::vec3 p1p2{ p2.x - p1.x, p2.y - p1.y, 0 };
		glm::vec3 q1q2{ q2.x - q1.x, q2.y - q1.y, 0 };

		// Cross product to determine if parallel 
		float denom = p1p2.x * q1q2.y - p1p2.y * q1q2.x;

		// Don't divide by zero
		if (std::abs(denom) > epsilon)
		{
			intersecting = true;

			glm::vec3 p1q1{ q1.x - p1.x, q1.y - p1.y, 0 };

			float num1 = p1q1.x * q1q2.y - p1q1.y * q1q2.x;
			float num2 = p1q1.x * p1p2.y - p1q1.y * p1p2.x;
			outLambda1 = num1 / denom;
			outLambda2 = num2 / denom;
		}
		else // are parallel
		{
			// Connect start points
			glm::vec3 p1q1{ p1.x - q1.x, p1.y - q1.y, 0 };

			// Cross product to determine if segments and the line connecting their start points are parallel, 
			// if so, than they are on a line
			// if not, then there is no intersection
			if (glm::abs(p1q1.x * q1q2.y - p1q1.y * q1q2.x) > epsilon)
			{
				return false;
			}

			// Check the 4 conditions
			outLambda1 = 0;
			outLambda2 = 0;
			if (IsPointOnLineSegment(p1, q1, q2) ||
				IsPointOnLineSegment(p2, q1, q2) ||
				IsPointOnLineSegment(q1, p1, p2) ||
				IsPointOnLineSegment(q2, p1, p2))
			{
				intersecting = true;
			}
		}
		return intersecting;
	}

	bool Raycast(const glm::vec3* vertices, const size_t nrVertices, const glm::vec2& rayP1, const glm::vec2& rayP2, HitInfo& hitInfo)
	{
		if (nrVertices == 0)
		{
			return false;
		}

		std::vector<HitInfo> hits;

		Rect r1{}, r2{};
		// r1: minimal AABB rect enclosing the ray
		r1.left = static_cast<int>(std::min(rayP1.x, rayP2.x));
		r1.bottom = static_cast<int>(std::min(rayP1.y, rayP2.y));
		r1.width = static_cast<int>(std::max(rayP1.x, rayP2.x) - r1.left);
		r1.height = static_cast<int>(std::max(rayP1.y, rayP2.y) - r1.bottom);

		// Line-line intersections.
		for (size_t idx{ 0 }; idx <= nrVertices; ++idx)
		{
			// Consider line segment between 2 consecutive vertices
			// (modulo to allow closed polygon, last - first vertice)
			auto a = vertices[0];
			auto b = vertices[1];
			auto c = vertices[2];
			auto d = vertices[3];
			glm::vec2 q1 = vertices[(idx + 0) % nrVertices];
			glm::vec2 q2 = vertices[(idx + 1) % nrVertices];

			// r2: minimal AABB rect enclosing the 2 vertices
			r2.left = static_cast<int>(std::min(q1.x, q2.x));
			r2.bottom = static_cast<int>(std::min(q1.y, q2.y));
			r2.width = static_cast<int>(std::max(q1.x, q2.x) - r2.left);
			r2.height = static_cast<int>(std::max(q1.y, q2.y) - r2.bottom);

			if (IsOverlapping(r1, r2))
			{
				float lambda1{};
				float lambda2{};

				if (IntersectLineSegments(rayP1, rayP2, q1, q2, lambda1, lambda2))
				{
					if (lambda1 > 0 && lambda1 <= 1 && lambda2 > 0 && lambda2 <= 1)
					{
						HitInfo linesHitInfo{};
						linesHitInfo.lambda = lambda1;
						linesHitInfo.intersectPoint = glm::vec2{ rayP1.x + ((rayP2.x - rayP1.x) * lambda1), rayP1.y + ((rayP2.y - rayP1.y) * lambda1) };
						glm::vec2 q1q2 = q2 - q1;
						glm::vec2 normal = glm::vec2(-q1q2.y, q1q2.x);
						linesHitInfo.normal = glm::normalize(normal);
						hits.push_back(linesHitInfo);
					}
				}
			}
		}

		if (hits.size() == 0)
		{
			return false;
		}

		// Get closest intersection point and copy it into the hitInfo parameter
		hitInfo = *std::min_element
		(
			hits.begin(), hits.end(),
			[](const HitInfo& first, const HitInfo& last)
			{
				return first.lambda < last.lambda;
			}
		);
		return true;
	}

	bool Raycast(const std::vector<glm::vec3>& vertices, const glm::vec2& rayP1, const glm::vec2& rayP2, HitInfo& hitInfo)
	{
		return Raycast(vertices.data(), vertices.size(), rayP1, rayP2, hitInfo);
	}
}