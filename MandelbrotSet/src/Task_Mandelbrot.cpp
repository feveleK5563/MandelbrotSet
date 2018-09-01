#include "Task_Mandelbrot.h"
#include "DxLib.h"
#include "InputState.h"
#include "ImageLoader.h"

namespace Mandelbrot
{
	std::weak_ptr<Resource> Resource::instance;
	//----------------------------------------------
	//���\�[�X�̃R���X�g���N�^
	Resource::Resource()
	{
		cursorImgName = "cursor";
		Image::imageLoader.LoadOneImage(cursorImgName, "data/image/cursor.png");
		cursorImgData = Image::imageLoader.GetImageData(cursorImgName);
	}
	//----------------------------------------------
	//���\�[�X�̃f�X�g���N�^
	Resource::~Resource()
	{
		Image::imageLoader.DeleteImageData(cursorImgName);
	}
	//----------------------------------------------
	//���\�[�X�̐���
	std::shared_ptr<Resource> Resource::Create()
	{
		auto sp = instance.lock();
		if (!sp)
		{
			sp = std::make_shared<Resource>();
			instance = sp;
		}
		return sp;
	}

	//��������������������������������������������������������������������������������������������
	//��������������������������������������������������������������������������������������������

	//----------------------------------------------
	//�^�X�N�̃R���X�g���N�^
	Task::Task():
		TaskAbstract(defGroupName, defTaskName, defPriority),
		res(Resource::Create()),
		isDrawGuide(true),
		cursor(res->cursorImgData),
		scale(4.0),
		posX(2.0),
		posY(2.0),
		nowPosX(0.0),
		nowPosY(0.0),
		posRevision((double)SystemDefine::windowSizeX / (double)SystemDefine::windowSizeY),
		size(SystemDefine::windowSizeY),
		minScale(scale),
		calculationNum(100)
	{
	}
	//----------------------------------------------
	//�^�X�N�̃f�X�g���N�^
	Task::~Task()
	{

	}
	//----------------------------------------------
	//�^�X�N�̐���
	std::shared_ptr<Task> Task::Create()
	{
		std::shared_ptr<Task> task = 
			std::make_shared<Task>();
		TS::taskSystem.AddTask(task);

		task->Initialize();
		return task;
	}

	//��������������������������������������������������������������������������������������������
	//��������������������������������������������������������������������������������������������

	//----------------------------------------------
	//����������
	//----------------------------------------------
	void Task::Initialize()
	{
		posX += posRevision;
		Reset();
	}

	//----------------------------------------------
	//�I������
	//----------------------------------------------
	void Task::Finalize()
	{

	}

	//----------------------------------------------
	//�X�V
	//----------------------------------------------
	void Task::Update()
	{
		ccCnt.Run();

		if (!ccCnt.IsTimeEnd())
		{
			CreateMandelbrot();
		}

		int mouse = GetMouseInput();
		//�J�[�\���ɂ��g����W�w��
		if (mouse & MOUSE_INPUT_LEFT)
		{
			cursor.on = true;
			int x, y;
			GetMousePoint(&x, &y);
			cursor.pos = { (float)x, (float)y };
			cursor.mposX = ConversionSetPos(x, posX);
			cursor.mposY = ConversionSetPos(y, posY);
		}

		//�K�C�h�̕\���؂�ւ�
		if (Input::key[KEY_INPUT_Q] == DOWN)
		{
			isDrawGuide = !isDrawGuide;
		}


		//�v�Z�ʂ̑���
		int plusCNum = GetMouseWheelRotVol();
		if (ccCnt.IsTimeEnd())
		{
			calculationNum = max(calculationNum + (plusCNum * 10), 10);
		}

		//�g��E�k��
		if (cursor.on)
		{
			if (Input::key[KEY_INPUT_A] == DOWN)
			{
				ChangeExpansionRate(10.0);
				return;
			}
			if (Input::key[KEY_INPUT_S] == DOWN)
			{
				ChangeExpansionRate(2.0);
				return;
			}
			if (Input::key[KEY_INPUT_D] == DOWN)
			{
				ChangeExpansionRate(1.25);
				return;
			}
			if (Input::key[KEY_INPUT_F] == DOWN)
			{
				ChangeExpansionRate(1.0);
				return;
			}

			if (Input::key[KEY_INPUT_Z] == DOWN)
			{
				ChangeExpansionRate(0.1);
				return;
			}
			if (Input::key[KEY_INPUT_X] == DOWN)
			{
				ChangeExpansionRate(0.5);
				return;
			}
			if (Input::key[KEY_INPUT_C] == DOWN)
			{
				ChangeExpansionRate(0.8);
				return;
			}
		}

		//�S�ă��Z�b�g
		if (Input::key[KEY_INPUT_SPACE] == DOWN)
		{
			if (ccCnt.IsTimeEnd())
			{
				calculationNum = 100;
				scale = 4.0;
				posX = 2.0 + posRevision;
				posY = 2.0;
				nowPosX = 0.0;
				nowPosY = 0.0;
				Reset();
			}
			else
			{
				while (!ccCnt.IsTimeEnd())
				{
					ccCnt.Run();
				}
			}
		}
	}

	//----------------------------------------------
	//�`��
	//----------------------------------------------
	void Task::Draw()
	{
		DrawMandelbrot();

		if (!isDrawGuide)
			return;

		int setPosY = 10;

		DrawFormatString(10, setPosY, GetColor(255, 255, 255),
			"�����W�FX(%f), Y(%f)", nowPosX, nowPosY);

		DrawFormatString(10, setPosY += 20, GetColor(255, 255, 255),
			"�v�Z�ʁF%d��", calculationNum);

		setPosY += 20;
		if (ccCnt.IsTimeEnd())
		{
			DrawFormatString(10, setPosY, GetColor(255, 255, 255), "�v�Z�I��");
		}
		else
		{
			DrawFormatString(10, setPosY, GetColor(255, 255, 255), "�v�Z���c");
		}

		DrawFormatString(10, setPosY += 25, GetColor(255, 255, 255), "�g�嗦 : %f", scale / minScale);

		if (cursor.on)
		{
			cursor.image.DrawOne(
				cursor.pos,
				1.f,
				0.f,
				false,
				0,
				Color(255, 255, 255, 255)
			);

			DrawFormatString(10, setPosY += 20, GetColor(255, 255, 255),
				"�w����W�FX(%f), Y(%f)", cursor.mposX, cursor.mposY);
		}
	}


	//----------------------------------------------
	//���Z�b�g
	void Task::Reset()
	{
		ccCnt.SetEndTime(calculationNum);
		ccCnt.ResetCntTime();
		cursor.on = false;
		for (int wy = 0; wy < SystemDefine::windowSizeY; ++wy)
		{
			for (int wx = 0; wx < SystemDefine::windowSizeX; ++wx)
			{
				color[wy][wx] = 0;
			}
		}
	}

	//----------------------------------------------
	//�}���f���u���W�����쐬����
	void Task::CreateMandelbrot()
	{
		for (int wy = 0; wy < SystemDefine::windowSizeY; ++wy)
		{
			for (int wx = 0; wx < SystemDefine::windowSizeX; ++wx)
			{
				if (color[wy][wx] == 0)
				{
					color[wy][wx] = MandelbrotCalculation(wx, wy, ccCnt.GetNowCntTime());
				}
			}
		}
	}

	//----------------------------------------------
	//�g�嗦��ύX����
	void Task::ChangeExpansionRate(double rate)
	{
		nowPosX = cursor.mposX;
		nowPosY = cursor.mposY;
		posX = -cursor.mposX + ((2.0 / rate) * (scale / minScale) * posRevision);
		posY = -cursor.mposY + ((2.0 / rate) * (scale / minScale));
		scale /= rate;
		Reset();
	}

	//----------------------------------------------
	//�w��s�N�Z���ʒu�̐F���v�Z�ɂ���Č��肷��
	int Task::MandelbrotCalculation(int wx, int wy, int time)
	{
		long double x = ConversionSetPos(wx, posX);	//�萔C�̎���
		long double y = ConversionSetPos(wy, posY);			//�萔C�̋���

		long double xz = 0.0;	//����Ԃ��v�Z�Ɏg�����f��z�̎���
		long double yz = 0.0;	//����Ԃ��v�Z�Ɏg�����f��z�̎���
		long double xn, yn;

		//calculationNum��v�Z���s��
		for (int i = 0; i <= time; ++i)
		{
			xn = xz * xz - yz * yz + x;	//z^2+C�̌v�Z(����)
			yn = 2.0 * xz * yz + y;		//z^2+C�̌v�Z(����)

			//������������z�̒l���X�V
			xz = xn;
			yz = yn;
		}

		//������Βl��2��(��Βl��2�悪4)�𒴂��Ă�����
		if (xz * xz + yz * yz > 4.0)
		{
			//���U����̂ŐF��ݒ肷��
			return GetColorToNumber(time);
		}

		//����������h��Ȃ�(��)
		return 0;
	}

	//----------------------------------------------
	//���������ɐF���擾
	int Task::GetColorToNumber(int cn)
	{
		return GetColor((cn % 32) * 8 + 1, cn % 256 + 1, cn % 256 + 1);
	}

	//----------------------------------------------
	//�w�萔�l��}��̍��W�̐��l�ɕϊ����ĕԂ�
	long double Task::ConversionSetPos(int num, double xory)
	{
		return (num * scale / size) - xory;
	}


	//----------------------------------------------
	//�}���f���u���W����`�悷��
	void Task::DrawMandelbrot()
	{
		for (int wy = 0; wy < SystemDefine::windowSizeY; ++wy)
		{
			for (int wx = 0; wx < SystemDefine::windowSizeX; ++wx)
			{
				if (color[wy][wx] != 0)
				{
					DrawPixel(wx, wy, color[wy][wx]);
				}
			}
		}
	}
}