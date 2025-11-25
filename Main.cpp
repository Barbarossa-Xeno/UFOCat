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
	FontAsset::Register(Util::FontFamily::YuseiMagic, FontMethod::SDF, 48, U"font/YuseiMagic-Regular.ttf");
	FontAsset::Register(Util::FontFamily::KoharuiroSunray, FontMethod::SDF, 48, U"font/GN-Koharuiro_Sunray.ttf");

	// バッファサイズの設定
	FontAsset(Util::FontFamily::YuseiMagic).setBufferThickness(4);
	FontAsset(Util::FontFamily::KoharuiroSunray).setBufferThickness(4);

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

	/*GUI::Scrollable s{ Vec2{ 10, 10 }, SizeF{ 500, 400 } };
	GUI::TextBox t{ }, t2{ }, t3{ };
	t.set(FontAsset(Util::FontFamily::YuseiMagic)(U"てすともじれつ"), 12).setPosition({ 0, 20 });
	t2.set(FontAsset(Util::FontFamily::KoharuiroSunray)(U"てすともじれつ"), 60).setPosition({ 0, 30 });
	t3.set(FontAsset(Util::FontFamily::YuseiMagic)(U"てすともじれつてすともじれつてすともじれつ"), 40).setPosition({ 0, 700 });
	
	s.setContents(t, t2, t3);*/
	
	while (System::Update())
	{
		if (not app.update())
		{
			break;
		}
		/*s.update();
		s.draw();*/
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
