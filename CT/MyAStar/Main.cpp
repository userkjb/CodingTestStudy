#include <iostream>
#include <vector>
#include <conio.h>
#include <list>

#include "EngineMath.h"

/// <summary>
/// 최상의 A-Star를 만드는 것이 아니다.
/// 배움에 목적을 둔 A-Star 이다.
/// </summary>


enum ETileType
{
	Ground,
	Wall,
	Start,
	End,
	OpenList,
};

enum ETileDir
{
	L,
	R,
	T,
	B,
	LT,
	LB,
	RT,
	RB,
	Max
};

class IntPoint
{
public :
	int X;
	int Y;

	IntPoint operator+(const IntPoint _Other)
	{
		return { _Other.X + X, _Other.Y + Y };
	}

	IntPoint operator=(const IntPoint _Other)
	{
		X = _Other.X;
		Y = _Other.Y;

		return *this;
	}

	bool operator==(const IntPoint _Other)
	{
		return _Other.X == X && _Other.Y == Y;
	}
};

class AStarInterface
{
public :
	// 여기 갈 수 있어?
	virtual bool IsCheck(IntPoint _Point) = 0; // 구현하면안됨

	// 나중에 삭제해야 하는 함수.
	virtual void SetTile(IntPoint _Point, ETileType _Type) = 0;
	virtual void Print() = 0;
};

class AStar
{
public :
	class AStarNode
	{
	public :
		AStarNode* Parent = nullptr;
		IntPoint Point;

		// 상세한 A-Star
		float G = 0.0f; // 여태까지 탐색을 거치면서 찾아온 길.
		float H = 0.0f; // End 와의 거리.
		float F = 0.0f; // 최종 합산.

		void Reset()
		{
			Parent = nullptr;

			G = 0.0f;
			H = 0.0f;
			F = 0.0f;
		}

		void Calculate(IntPoint _End)
		{
			float4 EndPos = float4(_End.X, _End.Y);
			float4 ThisPos = float4(Point.X, Point.Y);

			H = (EndPos - ThisPos).Size2D();

			if (nullptr != Parent)
			{
				G = Parent->G;
				// 부모와 나와의 거리 덧셈.
				G += (float4(Parent->Point.X, Parent->Point.Y) - float4(Point.X, Point.Y)).Size2D();
			}
			else
			{
				G = 0.0f;
			}

			F = H + G;
		}
	};

	~AStar()
	{
		for (AStarNode* Node : NodePool)
		{
			delete Node;
		}
	}

public :
	/// <summary>
	/// Node Create Setting
	/// </summary>
	/// <param name="_Interface">보통 TileMap</param>
	/// <param name="_Nodelimit"></param>
	void SetInit(AStarInterface* _Interface, int _Nodelimit)
	{
		TileInterface = _Interface;

		for (size_t i = 0; i < _Nodelimit; i++)
		{
			NodePool.push_back(new AStarNode()); // Object Pool - 메모리 최적화가 아니다.
		}
	}

	AStarNode* PopNode(IntPoint _Point, IntPoint _End, AStarNode* _Parent)
	{
		AStarNode* StartNode = NodePool.back();
		NodePool.pop_back();

		StartNode->Parent = _Parent;
		StartNode->Point = _Point;
		StartNode->Calculate(_End);

		return StartNode;
	}

	void NodeReset()
	{
		for (AStarNode* Node : OpenList)
		{
			NodePool.push_back(Node);
		}

		for (AStarNode* Node : CloseList)
		{
			NodePool.push_back(Node);
		}

		for (AStarNode* Node : NodePool)
		{
			Node->Reset();
		}
	}

	bool CheckOpenList(IntPoint _Point)
	{
		for (AStarNode* Node : OpenList)
		{
			if (Node->Point == _Point)
			{
				return true;
			}
		}
		return false;
	}

	bool CheckCloseList(IntPoint _Point)
	{
		for (AStarNode* Node : CloseList)
		{
			if (Node->Point == _Point)
			{
				return true;
			}
		}
		return false;
	}

	/// <summary>
	/// 8방향으로 주변을 탐색.
	/// </summary>
	/// <param name=""></param>
	AStarNode* CheckNode(AStarNode* _CurCheckNode, IntPoint _End)
	{
		static const IntPoint Dirs[static_cast<int>(ETileDir::Max)] =
		{
			{-1,  0},
			{ 1,  0},
			{ 0, -1},
			{ 0,  1},
			{-1, -1},
			{-1,  1},
			{ 1, -1},
			{ 1,  1},
		};

		for (int i = 0; i < static_cast<int>(ETileDir::Max); i++)
		{
			IntPoint CheckPoint = _CurCheckNode->Point + Dirs[i];

			if (false == TileInterface->IsCheck(CheckPoint))
			{
				continue;
			}

			if (true == CheckOpenList(CheckPoint))
			{
				continue;
			}

			if (true == CheckCloseList(CheckPoint))
			{
				continue;
			}

			PopNode(CheckPoint, _End, _CurCheckNode);
		}

		return nullptr;
	}

	void ListPrint()
	{
		for (AStarNode* Node : OpenList)
		{
			TileInterface->SetTile(Node->Point, ETileType::OpenList);
		}

		for (AStarNode* Node : CloseList)
		{
			TileInterface->SetTile(Node->Point, ETileType::OpenList);
		}

		TileInterface->Print();
	}

	std::vector<IntPoint> PathFind(IntPoint _Start, IntPoint _End)
	{
		// A-Star
		
		// 길 찾기 전에 해야 할 것.
		// 다시 원래대로 돌려야 한다.
		NodeReset();

		// 길찾기
		AStarNode* NewNode = PopNode(_Start, _End, nullptr);
		OpenList.push_back(NewNode);

		while (false == OpenList.empty())
		{
			// 정렬
			// 그냥 정렬하면 값으로 해버린다.
			// 주소값을 기준으로 정렬한다.
			// 원하는 것은 F가 작은 순으로 정렬되길 원한다.
			OpenList.sort([](AStarNode* _Left, AStarNode* _Right)
				{
					return _Left->F < _Right->F;
				});

			ListPrint();
			
			_getch();

			// 아래 노드가 다익스트라 알고리즘에 의해 End노드에
			// 가장 빠르게 도달할 가능성이 높은 노드이다.
			AStarNode* Node = OpenList.front();
			OpenList.pop_front();
			CloseList.push_back(Node);

			AStarNode* EndNode = CheckNode(Node, _End);

			if (nullptr != EndNode)
			{
				break;
			}
		}

		return std::vector<IntPoint>();
	}

private :
	// 미리 만들어 놓은 Node를 담는 List
	std::list<AStarNode*> NodePool; // vector로 사용하는 것이 좋지만 인터페이스가 좋지 않다.(앞, 뒤 내가 원하는 곳을 선택해서 구현하고자.)
	std::list<AStarNode*> OpenList;
	std::list<AStarNode*> CloseList;

	AStarInterface* TileInterface = nullptr;
};

/// <summary>
/// 게임이 바뀌면 매번 바뀐다.
/// </summary>
class TileMap : public AStarInterface
{
public :
	bool IsCheck(IntPoint _Point) override
	{
		if (TileMap[_Point.Y][_Point.X] == ETileType::Wall)
		{
			return false;
		}

		if (0 > _Point.Y)
		{
			return false;
		}

		if (0 > _Point.X)
		{
			return false;
		}

		if (_Point.Y >= TileMap.size())
		{
			return false;
		}

		if (_Point.X >= TileMap.size())
		{
			return false;
		}

		return true;
	}

	void Print()
	{
		system("cls");

		for (size_t y = 0; y < TileMap.size(); y++)
		{
			for (size_t x = 0; x < TileMap[y].size(); x++)
			{
				ETileType Type = TileMap[y][x];
				switch (Type)
				{
				case ETileType::Ground :
					std::cout << "*";
					break;
				case ETileType::Wall :
					std::cout << "#";
					break;
				case ETileType::Start:
					std::cout << "S";
					break;
				case ETileType::End:
					std::cout << "E";
					break;
				case ETileType::OpenList:
					std::cout << "O";
					break;
				default :
					break;
				}
			}

			std::cout << std::endl;
		}
	}

	void SetTile(IntPoint _Point, ETileType _Value)
	{
		TileMap[_Point.Y][_Point.X] = _Value;
	}

	void CreateMap(IntPoint _Scale)
	{
		Scale = _Scale;

		TileMap.resize(_Scale.Y);

		for (size_t y = 0; y < TileMap.size(); y++)
		{
			TileMap[y].resize(_Scale.X);

			for (size_t x = 0; x < TileMap[y].size(); x++)
			{
				TileMap[y][x] = ETileType::Ground;
			}
		}
	}

private :
	IntPoint Scale;
	std::vector<std::vector<ETileType>> TileMap;
};

int main()
{
	AStar NewAStar;

	TileMap NewTileMap;

	NewTileMap.CreateMap({ 30, 15 });
	NewAStar.SetInit(&NewTileMap, 100);

	for (int i = 0; i < 8; i++)
	{
		NewTileMap.SetTile({10, i + 3}, ETileType::Wall);
	}

	NewTileMap.SetTile({ 6, 7 }, ETileType::Start);
	NewTileMap.SetTile({ 15, 7 }, ETileType::End);

	NewTileMap.Print();

	_getch();

	std::vector<IntPoint> Result = NewAStar.PathFind({ 6, 7 }, { 15, 7 });
	
	std::cout << "Hello World" << std::endl;

	return 0;
}