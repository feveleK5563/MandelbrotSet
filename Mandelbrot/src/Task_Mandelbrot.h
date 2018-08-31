#pragma once
#include "TaskSystem.h"
#include "SystemDefine.h"
#include "TimeCounter.h"
#include "ImageDrawer.h"

namespace Mandelbrot
{
	const std::string	defGroupName	("�t���N�^���}�`");	//�O���[�v��
	const std::string	defTaskName		("�}���f���u���W��");	//�^�X�N��
	const float			defPriority		(0.f);	//�f�t�H���g�̏����D��x

	//----------------------------------------------
	class Resource
	{
	private:
		static std::weak_ptr<Resource> instance;
		std::string cursorImgName;

	public:
		ImageData	cursorImgData;

		Resource();		//�R���X�g���N�^
		~Resource();	//�f�X�g���N�^
		static std::shared_ptr<Resource> Create();	//���\�[�X�̐���
	};

	//----------------------------------------------
	class Task : public TaskAbstract
	{
	private:
		std::shared_ptr<Resource> res;	//�m�ۂ������\�[�X

		bool isDrawGuide;

		struct Cursor
		{
			bool		on;
			ImageDrawer	image;
			Math::Vec2	pos;
			double		mposX, mposY;

			Cursor(const ImageData& cursorImgData) :
				on(false),
				image(cursorImgData, true),
				pos(0.f, 0.f),
				mposX(0.0),
				mposY(0.0) {}
		};
		Cursor cursor;

		TimeCounter ccCnt;

		long double scale;
		long double posX, posY;

		const double posRevision;
		const double size;
		const double minScale;
		const int calculationNum;

		//�`��F
		int color[SystemDefine::windowSizeY][SystemDefine::windowSizeX];

	public:
		//�R���X�g���N�^
		Task();
		
		//�f�X�g���N�^
		~Task();
		
		//�^�X�N�̐���
		static std::shared_ptr<Task> Create();

		void Initialize() override;	//����������
		void Finalize() override;	//�I������
		void Update() override;		//�X�V
		void Draw() override;		//�`��

	private:
		//���Z�b�g
		void Reset();
		//�}���f���u���W�����쐬����
		void CreateMandelbrot();
		//�g�嗦��ύX����
		void ChangeExpansionRate(double rate);
		//�w��s�N�Z���ʒu�̐F���v�Z�ɂ���Č��肷��
		int MandelbrotCalculation(int wx, int wy, int time);
		//���������ɐF���擾
		int GetColorToNumber(int cn);
		//�w�萔�l��}��̍��W�̐��l�ɕϊ����ĕԂ�
		long double ConversionSetPos(int num, double xory);

		//�}���f���u���W����`�悷��
		void DrawMandelbrot();
	};
}