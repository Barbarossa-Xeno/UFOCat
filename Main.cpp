#include <Siv3D.hpp> // Siv3D v0.6.16
# include "Common.hpp"
# include "Title.hpp"
# include "Wanted.hpp"
# include "Level.hpp"
# include "Result.hpp"

using namespace UFOCat;

void Main()
{
	// TODO: 非同期ロードを実装してローディング画面を作ったほうがいい

	// フォントアセットの登録
	FontAsset::Register(Util::FontFamily::YuseiMagic, FontMethod::SDF, 48, U"font/YuseiMagic-Regular.ttf");
	FontAsset::Register(Util::FontFamily::KoharuiroSunray, FontMethod::SDF, 48, U"font/GN-Koharuiro_Sunray.ttf");

	// バッファサイズの設定
	FontAsset(Util::FontFamily::YuseiMagic).setBufferThickness(4);
	FontAsset(Util::FontFamily::KoharuiroSunray).setBufferThickness(4);

	// オーディオアセットの登録
	// TODO: 音量設定をつくるのもあり
	AudioAsset::Register(Util::AudioSource::BGM::Title, Audio::Stream, U"audio/recorderwofukuneko.mp3", Loop::Yes);
	AudioAsset::Register(Util::AudioSource::BGM::Level01, Audio::Stream, U"audio/魔王魂 ループ  サイバー29.mp3");
	AudioAsset::Register(Util::AudioSource::BGM::Level02, Audio::Stream, U"audio/魔王魂 ループ  サイバー41.mp3");
	AudioAsset::Register(Util::AudioSource::SE::Open, U"audio/パッ.mp3");
	AudioAsset::Register(Util::AudioSource::SE::OK, U"audio/決定ボタンを押す39.mp3");
	AudioAsset::Register(Util::AudioSource::SE::Cancel, U"audio/キャンセル4.mp3");
	AudioAsset::Register(Util::AudioSource::SE::Announce, U"audio/放送開始チャイム.mp3");
	AudioAsset::Register(Util::AudioSource::SE::CountDown, U"audio/パパッ.mp3");
	AudioAsset::Register(Util::AudioSource::SE::StartLevel, U"audio/警官のホイッスル1.mp3");
	AudioAsset::Register(Util::AudioSource::SE::FinishLevel, U"audio/警官のホイッスル2.mp3");
	AudioAsset::Register(Util::AudioSource::SE::Correct, U"audio/クイズ正解1.mp3");
	AudioAsset::Register(Util::AudioSource::SE::Incorrect, U"audio/クイズ不正解1.mp3");
	AudioAsset::Register(Util::AudioSource::SE::TimeUp, U"audio/試合終了のゴング.mp3");
	// ループのタイミング指定はサンプリング周波数を掛けて実際のサンプル数にしないといけないらしい by Google 検索の AI
	AudioAsset::Register(Util::AudioSource::SE::CountUpScore, U"audio/ドラムロール.mp3", Arg::loopBegin = static_cast<uint64>(0.683 * 44100), Arg::loopEnd = static_cast<uint64>(4.272 * 44100));
	AudioAsset::Register(Util::AudioSource::SE::FinishScore, U"audio/ロールの閉め.mp3");
	AudioAsset::Register(Util::AudioSource::SE::Cat01, U"audio/猫の鳴き声1.mp3");
	AudioAsset::Register(Util::AudioSource::SE::Cat02, U"audio/猫の鳴き声2.mp3");
	AudioAsset::Register(Util::AudioSource::SE::CatAngry, U"audio/猫の威嚇.mp3");

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
