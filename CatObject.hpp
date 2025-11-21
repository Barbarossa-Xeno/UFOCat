#pragma once
#include "CatData.hpp"
#include "Stopwatch.hpp"
#include "LevelData.hpp"

/// @brief インゲームを動作させるためのコア機能群
namespace UFOCat::Core
{
	/// @brief オブジェクトとしてのUFO猫のクラス @n
	/// ムーブ代入不可能
	class CatObject
	{
		/* -- クラスなど -- */

	private:

		/// @brief 外見の状態
		enum class AppearanceState
		{
			/// @brief 表示されていない（画面外もしくは描画していない）
			Hidden,
			/// @brief 画面内に登場している最中
			In,
			/// @brief 画面外へ退場している最中
			Out,
			/// @brief 完全に表示されている（画面内に）
			Visible
		};

		/// @brief 画面端の種類
		enum class ScreenEdgeDirection
		{
			Top,
			Right,
			Bottom,
			Left
		};

		/// @brief アクションに自動的に引数を入れて呼び出すための関数オブジェクト
		struct InvokeAction
		{
			/// @brief どの CatObject のインスタンスのアクションを呼び出すか
			CatObject *target;

			/// @brief コンストラクタ
			/// @param target アクションを呼び出す対象のインスタンス
			InvokeAction(CatObject &target) : target{ &target } {}

			/// @brief 関数を呼び出す
			/// @param value `CAct::ValidSignature` に適合している引数値
			/// @return ターゲットインスタンスの参照
			CatObject &operator()(const Action::ValidSignature auto &value) const;
		};

		/* -- フィールド -- */

	public:

		union
		{
			/// @brief 位置
			Vec2 position;
			struct
			{
				/// @brief X座標 ( = position.x )
				double x;

				/// @brief Y座標 ( = position.y )
				double y;
			};
		};

		union
		{
			/// @brief 速度
			Vec2 velocity;
			struct
			{
				/// @brief X方向の速さ ( = velocity.x )
				double vx;

				/// @brief Y方向の速さ ( = velocity.y )
				double vy;
			};
		};

	private:
		/// @brief 当たり判定の領域
		/// @note これもあくまで `m_ClientSize` を基準にしたときの当たり判定領域であって、表示スケールが変わった場合はこれも調整する必要がある
		Ellipse m_hitArea;

		/// @brief UFO猫のデータ
		CatData m_catData;

		/// @brief `appear()` などで出現時間の計算に使う内部ストップウォッチ @n
		/// @remarks 全ての時間を計測するメソッドで共有されるので、それらのメソッドを複数同時に呼び出してはいけない
		Util::Stopwatch m_stopwatch;

		/// @brief このオブジェクトがどんな外見状態にあるか
		AppearanceState m_appearanceState = AppearanceState::Hidden;

		/// @brief このオブジェクトが画面端のどこから出現するかを表す
		ScreenEdgeDirection m_edgeDirection = ScreenEdgeDirection::Top;

		/// @brief `cross()` で使うデータ群
		struct
		{
			/// @brief 画面外の始点
			Vec2 start = Vec2::Zero();

			/// @brief 画面外の終点
			Vec2 goal = Vec2::Zero();

			/// @brief 移動した回数
			int32 count = 0;

			/// @brief 始点と終点の間の距離を返す
			/// @return 始点と終点の間の距離
			double distance() const noexcept
			{
				return start.distanceFrom(goal);
			}

			/// @brief 始点ベクトルから終点ベクトルを引いた差のベクトルを返す
			/// @return 始点から終点を引いた差ベクトル
			Vec2 difference() const noexcept
			{
				return start - goal;
			}

		} m_crossData;

		/// @brief このUFO猫が行うアクションのデータ @n
		/// bound, cross, appear, appearFromEdge のいずれかを行うように設定されている
		LevelData::ActionData m_actionData;

		/// @brief `drawShadow()` を実行した際、オブジェクト背面に落とす影のスケール @n
		/// `drawShadow()` で引数の変更に応じて更新され続ける
		/// @note 画面外の座標を調整するのにつかう
		/// https://siv3d.github.io/ja-jp/tutorial3/render-texture/?h=%E5%BD%B1#5210-%E4%BB%BB%E6%84%8F%E5%BD%A2%E7%8A%B6%E3%81%AE%E3%82%B7%E3%83%A3%E3%83%89%E3%82%A6
		double m_shadowScale = 1.0;

		/// @brief 使用テクスチャ
		const Texture m_Texture;

		/// @brief スクリーンでの表示サイズ
		/// @note あくまでオブジェクトを湧かすときのサイズであって、それ以外の特別な用途では任意の倍率に拡大縮小してよい
		const SizeF m_ClientSize;

		/// @brief テクスチャのアルファ値
		double m_textureAlpha = 1;

		/// @brief 外から出てくるタイプのアクションで初期の出現範囲としてつかう描画領域外の場所
		const RectF m_screenEdgeArea;

		/// @brief テクスチャの表示領域
		/// @note 画像の上下部に余白を作ってしまっているので、そこを除いた領域を指定する
		const static Rect m_ClipArea;

		/// @brief テクスチャや図形などの描画スケール
		constexpr static double m_Scale = 0.3;

		/// @brief 当たり判定の大きさの倍率
		/// @note 1 でテクスチャと同じ大きさの楕円になる
		constexpr static double m_HitAreaScale = 0.8;

		/// @brief ドロップシャドウなどにつかうレンダーテクスチャ
		const struct
		{
			// シーン全体を白色で透明なレンダーテクスチャで覆う
			const RenderTexture ShadowTexture{ Scene::Size(), ColorF{ 1.0, 0.0 } };

			// 1 / 4 にダウンサンプリングする、ブラー用のレンダーテクスチャ
			const RenderTexture blur4{ ShadowTexture.size() / 4 };

			// ブラー用の中間テクスチャ
			const RenderTexture internal4{ ShadowTexture.size() / 4 };
		} m_renderTextures;

		/* -- ゲッター -- */

	public:

		/// @brief このオブジェクトのテクスチャを取得する
		/// @return テクスチャ
		Texture getTexture() const;

		/// @brief このオブジェクトのスクリーンでの表示サイズを取得する
		/// @return サイズ
		SizeF getClientSize() const;

		/// @brief このオブジェクトの当たり判定領域（楕円）を取得する
		/// @return 楕円オブジェクト
		Ellipse getHitArea() const;

		/// @brief このUFO猫の情報を取得する @n
		/// 出力先変数の指定が可能
		/// @param out 出力先変数（デフォルト `nullptr`） 新しいオブジェクトが格納される
		/// @return このオブジェクトが持つ CatData の読み取り専用左辺参照
		const CatData &getCatData(CatData *out = nullptr) const;

		/// @brief 左上を基準としたときにこのオブジェクトがはみ出さずに描画できる最大の領域を取得する
		/// @return 最大の領域を表す `Rect`
		Rect getMaxDisplayedArea() const;

		/* -- セッター -- */
	public:

		/// @brief 定式と引数の値に従ってランダムに速度を設定する
		/// @param level 整数値（1 ~ 10 の範囲で、特に現在のフェーズレベルを入れることを想定）
		/// @return 自分自身の参照
		CatObject &setRandomVelocity(size_t level);

		/// @brief UFO猫のデータを登録する
		/// @param data データ
		/// @return （変更を反映した）自分自身の参照
		CatObject &setCatData(CatData &&data);

		/// @brief UFO猫が行う動作（このクラスに定義された行動系メソッドのいずれかとそのオプション）を登録する
		/// @param actionData アクションデータ
		/// @return 自分自身の参照
		CatObject &setAction(const LevelData::ActionData &actionData);

		/* -- コンストラクタ -- */
	public:

		/// @brief 使用テクスチャからオブジェクトを作る
		/// @param texture 使用テクスチャ
		explicit CatObject(const Texture& texture)
			: m_Texture{ texture }
			// 半径は表示サイズの高さの半分、横幅に合わせてスケーリングした楕円を、さらに調整
			, m_hitArea{ Circle{ m_ClipArea.h * m_Scale / 2 }.scaled(static_cast<double>(m_ClipArea.w) / m_ClipArea.h, 1).scaled(m_HitAreaScale)}
			, m_ClientSize{ m_ClipArea.scaledAt(m_ClipArea.pos, m_Scale).size }
			// 自分のサイズ分だけ画面外に出した場所を原点として、
			// シーンの幅と高さにそれぞれ自分の縦横幅を足した範囲がちょうどぎりぎり表示されないところ
			, m_screenEdgeArea{ -Vec2(m_ClientSize), Scene::Width() + m_ClientSize.x, Scene::Height() + m_ClientSize.y }
			, velocity{ { 0, 0 } }
		{
			m_changeScreenEdgePosition();
		}

		/// @brief 使用テクスチャ、初期位置、初期速度からオブジェクトを作る
		/// @param texture 使用テクスチャ
		/// @param position 初期位置
		/// @param velocity 初期速度
		CatObject(const Texture& texture, const Vec2& position, const Vec2& velocity)
			: m_Texture{ texture }
			, m_hitArea{ Circle{ m_ClipArea.h * m_Scale / 2 }.scaled(static_cast<double>(m_ClipArea.w) / m_ClipArea.h, 1).scaled(m_HitAreaScale) }
			, m_ClientSize{ m_ClipArea.scaledAt(m_ClipArea.pos, m_Scale).size }
			, m_screenEdgeArea{ -Vec2(m_ClientSize), Scene::Width() + m_ClientSize.x, Scene::Height() + m_ClientSize.y }
			, position{ position }
			, velocity{ velocity }
		{ }

		/// @brief コピーコンストラクタ
		/// @param obj コピー元のオブジェクト
		/// @remarks すべてをコピーするわけではない
		CatObject(const CatObject &obj)
			: m_Texture{ obj.m_Texture }
			, m_ClientSize{ obj.m_ClientSize }
			, m_hitArea{ obj.m_hitArea }
			, m_screenEdgeArea{ obj.m_screenEdgeArea }
			, m_catData{ obj.m_catData }
			, m_actionData{ obj.m_actionData }
			, position{ obj.position }
			, velocity{ obj.velocity }
		{ }

		/* -- メソッド -- */

	public:

		// TODO: 線形移動以外を実現するには、速度を途中で変更できればいい

		/// @brief 画面端で跳ね返るようにする
		/// @return 自分自身の参照
		CatObject &bound();

		/// @brief 画面内を設定された速度 (`velocity`) で横切る
		/// @param period 出現周期（消えた状態から現れるまでの時間）
		/// @param count 横切る回数
		/// @return 自分自身の参照
		CatObject &cross(Duration period = 0s, uint32 count = std::numeric_limits<int32>::infinity());

		/// @brief 指定した範囲内のランダムな位置に指定した周期で出現し、指定したイージング関数と時間でそれぞれフェードインアウトする
		/// @param period 出現周期（消えている時間と現れている時間）
		/// @param fadeInFunc フェードインのイージング関数
		/// @param fadeIn フェードイン時間
		/// @param fadeOutFunc フェードアウトのイージング関数
		/// @param fadeOut フェードアウト時間
		/// @param range 出現範囲
		/// @return 自分自身の参照
		CatObject &appear(Duration period, Action::EasingFunction fadeInFunc, Duration fadeIn, Action::EasingFunction fadeOutFunc, Duration fadeOut, const Rect &range);

		/// @brief 指定した範囲内のランダムな位置に指定した周期で出現し、線形的にそれぞれの時間でフェードインアウトする
		/// @param period 出現周期（消えている時間と現れている時間）
		/// @param fadeIn フェードイン時間
		/// @param fadeOut フェードアウト時間
		/// @param range 出現範囲
		/// @return 自分自身の参照
		CatObject &appear(Duration period, Duration fadeIn, Duration fadeOut, const Rect &range);

		/// @brief 指定した範囲内のランダムな位置に指定した周期で出現し、指定した同じイージング関数と時間でフェードインアウトする
		/// @param period 出現周期（消えている時間と現れている時間）
		/// @param fadeFunc フェードインアウトのイージング関数
		/// @param fade フェードインアウト時間
		/// @param range 出現範囲
		/// @return 自分自身の参照
		CatObject &appear(Duration period, Action::EasingFunction fadeFunc, Duration fade, const Rect &range);

		/// @brief 指定した範囲内のランダムな位置に指定した周期で出現し、線形的に同じ時間でフェードインアウトする
		/// @param period 出現周期（消えている時間と現れている時間）
		/// @param fade フェードインアウト時間
		/// @param range 出現範囲
		/// @return 自分自身の参照
		CatObject &appear(Duration period, Duration fade, const Rect &range);

		/// @brief 自分自身が全て映る最大の範囲内のランダムな位置に指定した周期で出現し、指定したイージング関数と時間でそれぞれフェードインアウトする
		/// @param period 出現周期（消えている時間と現れている時間）
		/// @param fadeInFunc フェードインのイージング関数
		/// @param fadeIn フェードイン時間
		/// @param fadeOutFunc フェードアウトのイージング関数
		/// @param fadeOut フェードアウト時間
		/// @return 自分自身の参照
		CatObject &appear(Duration period, Action::EasingFunction fadeInFunc, Duration fadeIn, Action::EasingFunction fadeOutFunc, Duration fadeOut);

		/// @brief 自分自身が全て映る最大の範囲内のランダムな位置に指定した周期で出現し、線形的にそれぞれの時間でフェードインアウトする
		/// @param period 出現周期（消えている時間と現れている時間）
		/// @param fadeIn フェードイン時間
		/// @param fadeOut フェードアウト時間
		/// @return 自分自身の参照
		CatObject &appear(Duration period, Duration fadeIn, Duration fadeOut);

		/// @brief 自分自身が全て映る最大の範囲内のランダムな位置に指定した周期で出現し、指定した同じイージング関数と時間でフェードインアウトする
		/// @param period 出現周期（消えている時間と現れている時間）
		/// @param fadeFunc フェードインアウトのイージング関数
		/// @param fade フェードインアウト時間
		/// @return 自分自身の参照
		CatObject &appear(Duration period, Action::EasingFunction fadeFunc, Duration fade);

		/// @brief 自分自身が全て映る最大の範囲内のランダムな位置に指定した周期で出現し、線形的に同じ時間でフェードインアウトする
		/// @param period 出現周期（消えている時間と現れている時間）
		/// @param fade フェードインアウト時間
		/// @return 自分自身の参照
		CatObject &appear(Duration period, Duration fade);

		/// @brief 上、右、下、左側のうちいずれかの画面端から、指定したはみだし量の位置及び時間、出現周期でランダムで出現し、退去する @n
		/// 画面端からのはみだし量は 0 を指定するとその部分からは出現しなくなる
		/// @param period 出現周期（消えている時間と現れている時間）
		/// @param inFunc 出現時の移動モーションに使うイージング関数
		/// @param in 出現にかかる時間
		/// @param outFunc 退去時の移動モーションに使うイージング関数
		/// @param out 退去にかかる時間
		/// @param overflow 画面端からのはみだし量 サイズ4の `double` 配列で、0番目が上、1番目が右、2番目が下、3番目が左 のはみだし量を意味する
		/// @return 自分自身の参照
		CatObject &appearFromEdge(Duration period, Action::EasingFunction inFunc, Duration in, Action::EasingFunction outFunc, Duration out, const std::array<double, 4> &overflow);

		/// @brief 上、右、下、左側のうちいずれかの画面端から、指定したはみだし量の位置及び時間、出現周期でランダムで[線形的に]出現し、退去する @n
		/// 画面端からのはみだし量は 0 を指定するとその部分からは出現しなくなる
		/// @param period 出現周期（消えている時間と現れている時間）
		/// @param in 出現にかかる時間
		/// @param out 退去にかかる時間
		/// @param overflow 画面端からのはみだし量 サイズ4の `double` 配列で、0番目が上、1番目が右、2番目が下、3番目が左 のはみだし量を意味する
		/// @return 自分自身の参照
		CatObject &appearFromEdge(Duration period, Duration in, Duration out, const std::array<double, 4> &overflow);

		/// @brief 上、右、下、左側のうちいずれかの画面端から、指定したはみだし量の位置及び時間、出現周期でランダムで出現し、[同様に]退去する @n
		/// 画面端からのはみだし量は 0 を指定するとその部分からは出現しなくなる
		/// @param period 出現周期（消えている時間と現れている時間）
		/// @param inAndOutFunc 出現、退去時の移動モーションに使うイージング関数
		/// @param inAndOut 出現、退去にかかる時間
		/// @param overflow 画面端からのはみだし量 サイズ4の `double` 配列で、0番目が上、1番目が右、2番目が下、3番目が左 のはみだし量を意味する
		/// @return 自分自身の参照
		CatObject &appearFromEdge(Duration period, Action::EasingFunction inAndOutFunc, Duration inAndOut, const std::array<double, 4> &overflow);

		/// @brief 上、右、下、左側のうちいずれかの画面端から、指定したはみだし量の位置及び時間、出現周期でランダムで[線形的に]出現し、[同様に]退去する @n
		/// 画面端からのはみだし量は 0 を指定するとその部分からは出現しなくなる
		/// @param period 出現周期（消えている時間と現れている時間）
		/// @param inAndOut 出現、退去にかかる時間
		/// @param overflow 画面端からのはみだし量 サイズ4の `double` 配列で、0番目が上、1番目が右、2番目が下、3番目が左 のはみだし量を意味する
		/// @return 自分自身の参照
		CatObject &appearFromEdge(Duration period, Duration inAndOut, const std::array<double, 4> &overflow);

		// TODO: チャッピーとQiitaを参考に overflow~ を可変長引数で省略可能にもできるような宣言と定義をする！ -> 参考にしてもパラメータパックの意味がわからんくて無理
 		// 本当は Arg::top_, Arg::right_, Arg::bottom_, Arg::left_ を使いたかった

		/// @brief 登録されたアクションを実行する
		/// @details `m_actionData` に登録されたアクションを実行する
		/// @return 自分自身の参照
		CatObject &act();

		/// @brief オブジェクトを描画する
		/// @return 自分自身の参照
		CatObject &draw();

		/// @brief 影を描画する
		/// @return 自分自身の参照
		CatObject& drawShadow(ColorF color = ColorF{ 0.0, 0.5 }, Vec2 position = Vec2::Zero(), double scale = 1.05);

		/// @brief 当たり判定領域を描画する（デバッグ用）
		/// @return 自分自身の参照
		CatObject &drawHitArea();

		/// @brief オブジェクトが現在画面上に見えているかどうかを取得する
		/// @return 見えているなら `true`
		/// @return めちゃくちゃ正確とは限らない、あくまで内部で設定されている外見状態に基づく
		bool isVisible() const;

		/// @brief オブジェクトがタッチされたときに、現在のターゲット情報と比較して捕まえられるか試す
		/// @param target ターゲットの情報
		/// @param isCorrect タッチしたオブジェクトがターゲットと同じかどうかを格納する出力先変数へのポインタ
		/// @return タッチしたとき `true`
		bool checkCatchable(const CatData &target, bool *const isCorrect) const;

		/// @brief このオブジェクトの複製を生成して返す
		/// @return 情報がコピーされた新しいインスタンス
		CatObject clone() const;

	private:
		/// @brief 画面端のどこを開始点と終了点にするかランダムに決める @n
		/// 自身がぎりぎり映らない、表示領域外の場所として決められる @n
		/// 終了点は開始点の反対側として決められ、開始点と終了点の組み合わせをタプルで返す @n
		/// ** `m_edgeDirection` を同時に変更し、更に決まった開始点は、自動的に `position` に代入される **
		/// @return 開始点と終了点のタプル  [0] が開始点、[1] が終了点
		std::tuple<Vec2, Vec2> m_changeScreenEdgePosition();
	};
}
