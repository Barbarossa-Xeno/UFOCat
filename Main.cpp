# include <Siv3D.hpp> // Siv3D v0.6.16
# include "Source/Common.hpp"
# include "Source/Scene/Title.hpp"
# include "Source/Scene/Wanted.hpp"
# include "Source/Scene/Level.hpp"
# include "Source/Scene/Result.hpp"
# include "Source/Effect/Bubble.hpp"
# include "Source/Util/Stopwatch.hpp"

using namespace UFOCat;

void Main()
{
	// テクスチャフォルダから初めに全ての猫の画像アセットを登録
	for (auto &&[i, path] : Indexed(FileSystem::DirectoryContents(U"texture/cat")))
	{
		TextureAsset::Register(Cat(i), path);
	}

	// フォントアセットの登録
	FontAsset::Register(Util::FontName::YuseiMagic, FontMethod::SDF, 48, U"font/YuseiMagicRegular.ttf");
	FontAsset::Register(Util::FontName::KoharuiroSunray, FontMethod::SDF, 48, U"font/KoharuiroSunray.ttf");

	// バッファサイズの設定
	FontAsset(Util::FontName::YuseiMagic).setBufferThickness(4);
	FontAsset(Util::FontName::KoharuiroSunray).setBufferThickness(4);

	// オーディオアセットの登録
	// TODO: 音量設定をつくるのもあり
	AudioAsset::Register(Util::AudioName::BGM::Title, Audio::Stream, U"audio/title.mp3", Loop::Yes);
	AudioAsset::Register(Util::AudioName::BGM::Level01, Audio::Stream, U"audio/level01.mp3");
	AudioAsset::Register(Util::AudioName::BGM::Level02, Audio::Stream, U"audio/level02.mp3");
	AudioAsset::Register(Util::AudioName::SE::Open, U"audio/open.mp3");
	AudioAsset::Register(Util::AudioName::SE::OK, U"audio/ok.mp3");
	AudioAsset::Register(Util::AudioName::SE::Cancel, U"audio/cancenl.mp3");
	AudioAsset::Register(Util::AudioName::SE::Announce, U"audio/announce.mp3");
	AudioAsset::Register(Util::AudioName::SE::CountDown, U"audio/countDown.mp3");
	AudioAsset::Register(Util::AudioName::SE::StartLevel, U"audio/startLevel.mp3");
	AudioAsset::Register(Util::AudioName::SE::FinishLevel, U"audio/finishLevel.mp3");
	AudioAsset::Register(Util::AudioName::SE::Correct, U"audio/correct.mp3");
	AudioAsset::Register(Util::AudioName::SE::Incorrect, U"audio/incorrect.mp3");
	AudioAsset::Register(Util::AudioName::SE::TimeUp, U"audio/timeUp.mp3");
	// ループのタイミング指定は秒数にサンプリング周波数を掛けて実際のサンプル数にしないといけないらしい
	AudioAsset::Register(Util::AudioName::SE::CountUpScore, U"audio/countUpScore.mp3", Arg::loopBegin = static_cast<uint64>(0.683 * 44100), Arg::loopEnd = static_cast<uint64>(4.272 * 44100));
	AudioAsset::Register(Util::AudioName::SE::FinishScore, U"audio/finishScore.mp3");
	AudioAsset::Register(Util::AudioName::SE::Cat01, U"audio/cat01.mp3");
	AudioAsset::Register(Util::AudioName::SE::Cat02, U"audio/cat02.mp3");
	AudioAsset::Register(Util::AudioName::SE::CatAngry, U"audio/catAngry.mp3");

	// ウィンドウの設定
	Window::SetTitle(U"UFO猫をつかまえろ!!");
	Window::SetStyle(WindowStyle::Sizable);

	Scene::SetResizeMode(ResizeMode::Keep);

	App app;

	app.add<Title>(SceneState::Title);
	app.add<Wanted>(SceneState::Wanted);
	app.add<Level>(SceneState::Level);
	app.add<Result>(SceneState::Result);

	app.init(SceneState::Title, 1s);

	s3d::Effect effect;
	Util::Stopwatch effectTimer;
	
	while (System::Update())
	{
		if (not app.update())
		{
			break;
		}

		// タップエフェクト
		{
			if (MouseL.down())
			{
				effect.add<UFOCat::Effect::Bubble>(Cursor::Pos(), 5, 15, 1s, Random(0.0, 360.0), Random(0.2, 0.4));
			}
			if (MouseL.pressed())
			{
				effectTimer.setInterval([&]() { effect.add<UFOCat::Effect::Bubble>(Cursor::Pos(), 6, 15, 0.7s, Random(0.0, 360.0), Random(0.2, 0.4)); }, 0.3s);
			}

			effect.update();
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
