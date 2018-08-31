#pragma once
#include "TaskSystem.h"
#include "SystemDefine.h"
#include "TimeCounter.h"
#include "ImageDrawer.h"

namespace Mandelbrot
{
	const std::string	defGroupName	("フラクタル図形");	//グループ名
	const std::string	defTaskName		("マンデルブロ集合");	//タスク名
	const float			defPriority		(0.f);	//デフォルトの処理優先度

	//----------------------------------------------
	class Resource
	{
	private:
		static std::weak_ptr<Resource> instance;
		std::string cursorImgName;

	public:
		ImageData	cursorImgData;

		Resource();		//コンストラクタ
		~Resource();	//デストラクタ
		static std::shared_ptr<Resource> Create();	//リソースの生成
	};

	//----------------------------------------------
	class Task : public TaskAbstract
	{
	private:
		std::shared_ptr<Resource> res;	//確保したリソース

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

		//描画色
		int color[SystemDefine::windowSizeY][SystemDefine::windowSizeX];

	public:
		//コンストラクタ
		Task();
		
		//デストラクタ
		~Task();
		
		//タスクの生成
		static std::shared_ptr<Task> Create();

		void Initialize() override;	//初期化処理
		void Finalize() override;	//終了処理
		void Update() override;		//更新
		void Draw() override;		//描画

	private:
		//リセット
		void Reset();
		//マンデルブロ集合を作成する
		void CreateMandelbrot();
		//拡大率を変更する
		void ChangeExpansionRate(double rate);
		//指定ピクセル位置の色を計算によって決定する
		int MandelbrotCalculation(int wx, int wy, int time);
		//数字を元に色を取得
		int GetColorToNumber(int cn);
		//指定数値を図上の座標の数値に変換して返す
		long double ConversionSetPos(int num, double xory);

		//マンデルブロ集合を描画する
		void DrawMandelbrot();
	};
}