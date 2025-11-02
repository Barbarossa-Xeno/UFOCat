#include <Siv3D.hpp> // Siv3D v0.6.16
# include "Common.hpp"
# include "Title.hpp"
# include "Wanted.hpp"
# include "Level.hpp"
# include "Result.hpp"

using namespace UFOCat;

void Main()
{
	// フォントアセットの登録
	FontAsset::Register(FontName::YuseiMagic, FontMethod::SDF, 48, U"font/YuseiMagic-Regular.ttf");
	FontAsset::Register(FontName::KoharuiroSunray, FontMethod::SDF, 48, U"font/GN-Koharuiro_Sunray.ttf");

	// バッファサイズの設定
	FontAsset(FontName::YuseiMagic).setBufferThickness(4);
	FontAsset(FontName::KoharuiroSunray).setBufferThickness(4);

	// ウィンドウの設定
	Window::SetTitle(U"UFO猫をつかまえろ!!");
	Window::SetStyle(WindowStyle::Sizable);

	Scene::SetResizeMode(ResizeMode::Keep);

	App app;

	app.add<Title>(State::Title);
	app.add<Wanted>(State::Wanted);
	app.add<Level>(State::Level);
	app.add<Result>(State::Result);

	app.init(State::Title, 1s);
	
	while (System::Update())
	{
		if (not app.update())
		{
			break;
		}
	}
}

//
// - Debug ビルド: プログラムの最適化を減らす代わりに、エラーやクラッシュ時に詳細な情報を得られます。
//
// - Release ビルド: 最大限の最適化でビルドします。
//
// - [デバッグ] メニュー → [デバッグの開始] でプログラムを実行すると、[出力] ウィンドウに詳細なログが表示され、エラーの原因を探せます。
//
// - Visual Studio を更新した直後は、プログラムのリビルド（[ビルド]メニュー → [ソリューションのリビルド]）が必要な場合があります。
//
