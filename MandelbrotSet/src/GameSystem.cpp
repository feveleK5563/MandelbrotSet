#include "GameSystem.h"
#include "DxLib.h"
#include "Fps.h"
#include "SystemDefine.h"
#include "InputState.h"
#include "TaskSystem.h"
#include "Task_Mandelbrot.h"

GameSystem::GameSystem(){}

//-----------------------------------------------------------------------------
//����������
void GameSystem::Initialize()
{
	SetOutApplicationLogValidFlag(false);//Log.txt������Ȃ��悤�ɂ���

	//�E�B���h�E���[�h
	ChangeWindowMode(TRUE);
	//�E�C���h�E�̃T�C�Y���蓮�ŕύX�ł����A���E�C���h�E�̃T�C�Y�ɍ��킹�Ċg������Ȃ��悤�ɂ���
	SetWindowSizeChangeEnableFlag(FALSE, FALSE);
	//�E�B���h�E�T�C�Y(�𑜓x�ȉ��ɐݒ�)
	SetWindowSize(SystemDefine::windowSizeX, SystemDefine::windowSizeY);

	//��ʉ𑜓x�ƃJ���[�r�b�g��
	SetGraphMode(SystemDefine::windowSizeX, SystemDefine::windowSizeY, 32);
	//�E�B���h�E�^�C�g����t����
	SetWindowText("Mandelbrot");

	//�������Ɨ���ʉ�
	if (DxLib_Init() == -1 || SetDrawScreen(DX_SCREEN_BACK) != 0)
	{
		return;
	}

	FirstCreateTask();
}

//-----------------------------------------------------------------------------
//���C�����[�v
void GameSystem::MainLoop()
{
	Fps fps;

	while (Run())
	{
		fps.Wait();
		fps.Update();
		//�^�X�N�����݂��Ȃ������ꍇ�I������
		if (!TS::taskSystem.Update())
		{
			break;
		}

		TS::taskSystem.Draw();
		//fps.Draw();
	}
	TS::taskSystem.AllDeleteTask();
}

//-----------------------------------------------------------------------------
//�I������
void GameSystem::Finalize()
{
	DxLib_End();
}

//-----------------------------------------------------------------------------
//�t���X�N���[�����[�h�ɂ��邩�ۂ������b�Z�[�W�{�b�N�X�Ŗ₤
bool GameSystem::IsFullScreenMode()
{
	int flag;
	flag = MessageBox(
				NULL,
				TEXT("�t���X�N���[�����[�h�ŋN�����܂����H"),
				TEXT("�X�N���[���ݒ�"),
				MB_YESNO | MB_ICONQUESTION);

	if (flag == IDYES)
	{
		return true;
	}

	return false;
}

//-----------------------------------------------------------------------------
//���[�v���񂷍ۂ̔��菈��
bool GameSystem::Run()
{
	return	ScreenFlip() == 0 &&					//����ʂ�\��ʂɔ��f
		ProcessMessage() == 0 &&					//���b�Z�[�W����
		ClearDrawScreen() == 0 &&					//��ʂ��N���A
		Input::GetInputStateAll() &&				//���͏����擾
		Input::key[KEY_INPUT_ESCAPE] == OFF;		//ESC��������Ă��Ȃ�
}

//-----------------------------------------------------------------------------
//�ŏ��ɍ쐬����^�X�N
void GameSystem::FirstCreateTask()
{
	//�ŏ��ɍ쐬����^�X�N�������ɓ���
	Mandelbrot::Task::Create();
}