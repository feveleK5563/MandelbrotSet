#include "Task_Mandelbrot.h"
#include "DxLib.h"
#include "InputState.h"
#include "ImageLoader.h"

namespace Mandelbrot
{
	std::weak_ptr<Resource> Resource::instance;
	//----------------------------------------------
	//リソースのコンストラクタ
	Resource::Resource()
	{
		cursorImgName = "cursor";
		Image::imageLoader.LoadOneImage(cursorImgName, "data/image/cursor.png");
		cursorImgData = Image::imageLoader.GetImageData(cursorImgName);
	}
	//----------------------------------------------
	//リソースのデストラクタ
	Resource::~Resource()
	{
		Image::imageLoader.DeleteImageData(cursorImgName);
	}
	//----------------------------------------------
	//リソースの生成
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

	//☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★
	//★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆

	//----------------------------------------------
	//タスクのコンストラクタ
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
	//タスクのデストラクタ
	Task::~Task()
	{

	}
	//----------------------------------------------
	//タスクの生成
	std::shared_ptr<Task> Task::Create()
	{
		std::shared_ptr<Task> task = 
			std::make_shared<Task>();
		TS::taskSystem.AddTask(task);

		task->Initialize();
		return task;
	}

	//◇◆◇◆◇◆◇◆◇◆◇◆◇◆◇◆◇◆◇◆◇◆◇◆◇◆◇◆◇◆◇◆◇◆◇◆◇◆◇◆◇◆◇◆◇◆
	//◆◇◆◇◆◇◆◇◆◇◆◇◆◇◆◇◆◇◆◇◆◇◆◇◆◇◆◇◆◇◆◇◆◇◆◇◆◇◆◇◆◇◆◇◆◇

	//----------------------------------------------
	//初期化処理
	//----------------------------------------------
	void Task::Initialize()
	{
		posX += posRevision;
		Reset();
	}

	//----------------------------------------------
	//終了処理
	//----------------------------------------------
	void Task::Finalize()
	{

	}

	//----------------------------------------------
	//更新
	//----------------------------------------------
	void Task::Update()
	{
		ccCnt.Run();

		if (!ccCnt.IsTimeEnd())
		{
			CreateMandelbrot();
		}

		int mouse = GetMouseInput();
		//カーソルによる拡大座標指定
		if (mouse & MOUSE_INPUT_LEFT)
		{
			cursor.on = true;
			int x, y;
			GetMousePoint(&x, &y);
			cursor.pos = { (float)x, (float)y };
			cursor.mposX = ConversionSetPos(x, posX);
			cursor.mposY = ConversionSetPos(y, posY);
		}

		//ガイドの表示切り替え
		if (Input::key[KEY_INPUT_Q] == DOWN)
		{
			isDrawGuide = !isDrawGuide;
		}


		//計算量の増減
		int plusCNum = GetMouseWheelRotVol();
		if (ccCnt.IsTimeEnd())
		{
			calculationNum = max(calculationNum + (plusCNum * 10), 10);
		}

		//拡大・縮小
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

		//全てリセット
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
	//描画
	//----------------------------------------------
	void Task::Draw()
	{
		DrawMandelbrot();

		if (!isDrawGuide)
			return;

		int setPosY = 10;

		DrawFormatString(10, setPosY, GetColor(255, 255, 255),
			"現座標：X(%f), Y(%f)", nowPosX, nowPosY);

		DrawFormatString(10, setPosY += 20, GetColor(255, 255, 255),
			"計算量：%d回", calculationNum);

		setPosY += 20;
		if (ccCnt.IsTimeEnd())
		{
			DrawFormatString(10, setPosY, GetColor(255, 255, 255), "計算終了");
		}
		else
		{
			DrawFormatString(10, setPosY, GetColor(255, 255, 255), "計算中…");
		}

		DrawFormatString(10, setPosY += 25, GetColor(255, 255, 255), "拡大率 : %f", scale / minScale);

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
				"指定座標：X(%f), Y(%f)", cursor.mposX, cursor.mposY);
		}
	}


	//----------------------------------------------
	//リセット
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
	//マンデルブロ集合を作成する
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
	//拡大率を変更する
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
	//指定ピクセル位置の色を計算によって決定する
	int Task::MandelbrotCalculation(int wx, int wy, int time)
	{
		long double x = ConversionSetPos(wx, posX);	//定数Cの実部
		long double y = ConversionSetPos(wy, posY);			//定数Cの虚部

		long double xz = 0.0;	//くり返し計算に使う複素数zの実部
		long double yz = 0.0;	//くり返し計算に使う複素数zの実部
		long double xn, yn;

		//calculationNum回計算を行う
		for (int i = 0; i <= time; ++i)
		{
			xn = xz * xz - yz * yz + x;	//z^2+Cの計算(実部)
			yn = 2.0 * xz * yz + y;		//z^2+Cの計算(虚部)

			//実部虚部共にzの値を更新
			xz = xn;
			yz = yn;
		}

		//もし絶対値が2を(絶対値の2乗が4)を超えていたら
		if (xz * xz + yz * yz > 4.0)
		{
			//発散するので色を設定する
			return GetColorToNumber(time);
		}

		//収束したら塗らない(黒)
		return 0;
	}

	//----------------------------------------------
	//数字を元に色を取得
	int Task::GetColorToNumber(int cn)
	{
		return GetColor((cn % 32) * 8 + 1, cn % 256 + 1, cn % 256 + 1);
	}

	//----------------------------------------------
	//指定数値を図上の座標の数値に変換して返す
	long double Task::ConversionSetPos(int num, double xory)
	{
		return (num * scale / size) - xory;
	}


	//----------------------------------------------
	//マンデルブロ集合を描画する
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