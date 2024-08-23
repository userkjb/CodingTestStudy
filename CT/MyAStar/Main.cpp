#include <iostream>
#include <vector>
#include <conio.h>
#include <list>

#include "EngineMath.h"

/// <summary>
/// �ֻ��� A-Star�� ����� ���� �ƴϴ�.
/// ��� ������ �� A-Star �̴�.
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
	// ���� �� �� �־�?
	virtual bool IsCheck(IntPoint _Point) = 0; // �����ϸ�ȵ�

	// ���߿� �����ؾ� �ϴ� �Լ�.
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

		// ���� A-Star
		float G = 0.0f; // ���±��� Ž���� ��ġ�鼭 ã�ƿ� ��.
		float H = 0.0f; // End ���� �Ÿ�.
		float F = 0.0f; // ���� �ջ�.

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
				// �θ�� ������ �Ÿ� ����.
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
	/// <param name="_Interface">���� TileMap</param>
	/// <param name="_Nodelimit"></param>
	void SetInit(AStarInterface* _Interface, int _Nodelimit)
	{
		TileInterface = _Interface;

		for (size_t i = 0; i < _Nodelimit; i++)
		{
			NodePool.push_back(new AStarNode()); // Object Pool - �޸� ����ȭ�� �ƴϴ�.
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
	/// 8�������� �ֺ��� Ž��.
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
		
		// �� ã�� ���� �ؾ� �� ��.
		// �ٽ� ������� ������ �Ѵ�.
		NodeReset();

		// ��ã��
		AStarNode* NewNode = PopNode(_Start, _End, nullptr);
		OpenList.push_back(NewNode);

		while (false == OpenList.empty())
		{
			// ����
			// �׳� �����ϸ� ������ �ع�����.
			// �ּҰ��� �������� �����Ѵ�.
			// ���ϴ� ���� F�� ���� ������ ���ĵǱ� ���Ѵ�.
			OpenList.sort([](AStarNode* _Left, AStarNode* _Right)
				{
					return _Left->F < _Right->F;
				});

			ListPrint();
			
			_getch();

			// �Ʒ� ��尡 ���ͽ�Ʈ�� �˰��� ���� End��忡
			// ���� ������ ������ ���ɼ��� ���� ����̴�.
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
	// �̸� ����� ���� Node�� ��� List
	std::list<AStarNode*> NodePool; // vector�� ����ϴ� ���� ������ �������̽��� ���� �ʴ�.(��, �� ���� ���ϴ� ���� �����ؼ� �����ϰ���.)
	std::list<AStarNode*> OpenList;
	std::list<AStarNode*> CloseList;

	AStarInterface* TileInterface = nullptr;
};

/// <summary>
/// ������ �ٲ�� �Ź� �ٲ��.
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