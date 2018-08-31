#include "SystemDefine.h"

namespace SystemDefine
{
	//��ʊO����(�����ɉ摜�T�C�Y���w��A��ʊO�ɏo�Ă�����true)
	bool WindowOutBox(const Math::Box2D& hitBase)
	{
		Math::Box2D windowBox(0, 0, windowSizeX, windowSizeY);
		return !windowBox.IsHit(hitBase);
	}

	//��ʓ�����(�����ɉ摜�T�C�Y���w��A��ʓ��ɂ�����true)
	bool WindowInBox(const Math::Box2D& hitBase)
	{
		Math::Box2D windowBox(0, 0, windowSizeX, windowSizeY);
		return windowBox.IsIn(hitBase);
	}

	//���|�C���^�����S��delete����
	template<class T>void SafeDelete(T& t)
	{
		if (t != nullptr)
		{
			delete t;
			t = nullptr;
		}
	}

	//���|�C���^�����S��delete����(�z���)
	template<class T>void SafeDeleteArr(T& t)
	{
		if (t != nullptr)
		{
			delete[] t;
			t = nullptr;
		}
	}

	std::random_device rd;
	std::mt19937 mtrand(rd());

	//�͈͓��Ń����_���Ȓl���w������擾����
	int  GetRand(int lowerLimit, int upperLimit)
	{
		std::uniform_int_distribution<> randRange(lowerLimit, upperLimit);
		return randRange(mtrand);
	}
}