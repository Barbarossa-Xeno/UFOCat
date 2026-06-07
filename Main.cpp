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
	// 埋め込みリソースにしたテクスチャフォルダから全ての猫の画像アセットを登録
	for (auto &&[i, path] : Indexed(EnumResourceFiles().filter([](const auto &path) { return path.lowercased().contains(U"texture/cat"); })))
	{
		TextureAsset::Register(Cat(i), Resource(path));
	}

	// 埋め込みリソースにしたフォントアセットの登録
	FontAsset::Register(Util::FontName::YuseiMagic, FontMethod::SDF, 48, Resource(U"font/YuseiMagicRegular.ttf"));
	FontAsset::Register(Util::FontName::KoharuiroSunray, FontMethod::SDF, 48, Resource(U"font/KoharuiroSunray.ttf"));

	// バッファサイズの設定
	FontAsset(Util::FontName::YuseiMagic).setBufferThickness(4);
	FontAsset(Util::FontName::KoharuiroSunray).setBufferThickness(4);

	// オーディオアセットの登録
	// TODO: 音量設定をつくるのもあり
	AudioAsset::Register(Util::AudioName::BGM::Title, Audio::Stream, Resource(U"audio/title.mp3"), Loop::Yes);
	AudioAsset::Register(Util::AudioName::BGM::Level01, Audio::Stream, Resource(U"audio/level01.mp3"));
	AudioAsset::Register(Util::AudioName::BGM::Level02, Audio::Stream, Resource(U"audio/level02.mp3"));
	AudioAsset::Register(Util::AudioName::SE::Open, Resource(U"audio/open.mp3"));
	AudioAsset::Register(Util::AudioName::SE::OK, Resource(U"audio/ok.mp3"));
	AudioAsset::Register(Util::AudioName::SE::Cancel, Resource(U"audio/cancel.mp3"));
	AudioAsset::Register(Util::AudioName::SE::Announce, Resource(U"audio/announce.mp3"));
	AudioAsset::Register(Util::AudioName::SE::CountDown, Resource(U"audio/countDown.mp3"));
	AudioAsset::Register(Util::AudioName::SE::StartLevel, Resource(U"audio/startLevel.mp3"));
	AudioAsset::Register(Util::AudioName::SE::FinishLevel, Resource(U"audio/finishLevel.mp3"));
	AudioAsset::Register(Util::AudioName::SE::Correct, Resource(U"audio/correct.mp3"));
	AudioAsset::Register(Util::AudioName::SE::Incorrect, Resource(U"audio/incorrect.mp3"));
	AudioAsset::Register(Util::AudioName::SE::TimeUp, Resource(U"audio/timeUp.mp3"));
	// ループのタイミング指定は秒数にサンプリング周波数を掛けて実際のサンプル数にしないといけないらしい
	AudioAsset::Register(Util::AudioName::SE::CountUpScore, Resource(U"audio/countUpScore.mp3"), Arg::loopBegin = static_cast<uint64>(0.683 * 44100), Arg::loopEnd = static_cast<uint64>(4.272 * 44100));
	AudioAsset::Register(Util::AudioName::SE::FinishScore, Resource(U"audio/finishScore.mp3"));
	AudioAsset::Register(Util::AudioName::SE::Cat01, Resource(U"audio/cat01.mp3"));
	AudioAsset::Register(Util::AudioName::SE::Cat02, Resource(U"audio/cat02.mp3"));
	AudioAsset::Register(Util::AudioName::SE::CatAngry, Resource(U"audio/catAngry.mp3"));

	// ウィンドウの設定
	Window::SetTitle(U"UFO猫をつかまえろ!!");
	Window::SetStyle(WindowStyle::Sizable);
	Scene::SetResizeMode(ResizeMode::Keep);

	// アプリケーションの初期化
	App app;
	app.add<Title>(SceneState::Title);
	app.add<Wanted>(SceneState::Wanted);
	app.add<Level>(SceneState::Level);
	app.add<Result>(SceneState::Result);
	app.init(SceneState::Title, 1s);

	/// @brief タップエフェクト
	s3d::Effect tapEffect;

	/// @brief 長押しした時の時間を図ってタップエフェクトを制御するタイマー
	Util::Stopwatch tapEffectTimer;

	// メインループ
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
				// タップされた瞬間に生成
				tapEffect.add<UFOCat::Effect::Bubble>(Cursor::Pos(), 5, 15, 1s, Random(0.0, 360.0), Random(0.2, 0.4));
			}
			if (MouseL.pressed())
			{
				// 長押ししている間は、0.3秒ごとに新しいエフェクトを生成し、0.7秒で消えるようにする
				tapEffectTimer.setInterval([&]() { tapEffect.add<UFOCat::Effect::Bubble>(Cursor::Pos(), 6, 15, 0.7s, Random(0.0, 360.0), Random(0.2, 0.4)); }, 0.3s);
			}

			// 更新と描画
			tapEffect.update();
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
