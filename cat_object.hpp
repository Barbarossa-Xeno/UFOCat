#pragma once
#include "cat_action.hpp"
#include "cat_data.hpp"
#include "delta_stopwatch.hpp"
#include "phase.hpp"
#include <Siv3D.hpp>

/// @brief UFO猫のクラス
class CatObject
{
	/* -- クラスなど -- */

private:

	/// @brief 外見の状態
	enum class Appearance
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
	enum class ScreenEdge
	{
		Top,
		Right,
		Bottom,
		Left
	};

	/* -- エイリアス -- */
public:

	/// @brief Siv3D のイージング関数の型
	using EasingFunction = cact::EasingFunction;

	/* -- フィールド -- */

public:

	/// @brief 使用テクスチャ
	const Texture texture;

	/// @brief スクリーンでの表示サイズ
	const SizeF ClientSize;

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
	Ellipse m_hitArea;

	/// @brief UFO猫のデータ
	CatData m_catData;

	/// @brief `appear()` などで出現時間の計算に使う内部ストップウォッチ @n
	/// @remarks 全ての時間を計測するメソッドで共有されるので、それらのメソッドを複数同時に呼び出してはいけない
	DeltaStopwatch m_stopwatch;

	/// @brief このオブジェクトがどんな外見状態にあるか
	Appearance m_appearanceState = Appearance::Hidden;

	/// @brief このオブジェクトが画面端のどこから出現するかを表す
	ScreenEdge m_edgeState = ScreenEdge::Top;

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

	std::function<CatObject&(void)> m_action;

	/// @brief テクスチャのアルファ値
	double m_textureAlpha = 1;

	/// @brief テクスチャの表示領域
	/// @note 画像の上下部に余白を作ってしまっているので、そこを除いた領域を指定する
	const static Rect m_ClipArea;

	/// @brief テクスチャや図形などの描画スケール
	constexpr static double m_Scale = 0.35;

	/* -- ゲッター -- */

public:

	/// @brief このオブジェクトの当たり判定領域（楕円）を取得する
	/// @return 楕円オブジェクト
	Ellipse getHitArea() const;

	/// @brief このUFO猫の情報を取得する @n
	/// 出力先変数の指定が可能
	/// @param out 出力先変数（デフォルト `nullptr`） 新しいオブジェクトが格納される
	/// @return このオブジェクトが持つ CatData の読み取り専用左辺参照
	const CatData& getCatData(CatData *out = nullptr) const;

	/// @brief 左上を基準としたときにこのオブジェクトがはみ出さずに描画できる最大の領域を取得する
	/// @return 最大の領域を表す `Rect`
	Rect getMaxDisplayedArea() const;

	/* -- セッター -- */
public:

	/// @brief UFO猫のデータを登録する
	/// @param data データ
	/// @return （変更を反映した）自分自身の参照
	CatObject& setCatData(CatData&& data);

	CatObject& setAction(Phase::ActionData actionData);

private:

	/// @brief クラス全体で共有するターゲット情報を設定する
	/// @param data ターゲット情報
	static void SetTargetData(const CatData& data);

	/* -- コンストラクタ -- */
	// TODO: 何故かコピーコンストラクタが暗黙定義されないので、自分で試してみる
public:

	/// @brief 使用テクスチャからオブジェクトを作る
	/// @param texture 使用テクスチャ
	explicit CatObject(const Texture& texture)
		: texture{ texture }
		, m_hitArea{ Circle{ m_ClipArea.h * m_Scale / 2 }.scaled(static_cast<double>(m_ClipArea.w) / m_ClipArea.h, 1) }
		, ClientSize{ m_ClipArea.scaledAt(m_ClipArea.pos, m_Scale).size }
		, position{ { 0, 0 } }
		, velocity{ { 0, 0 } }
	{ }

	/// @brief 使用テクスチャ、初期位置、初期速度からオブジェクトを作る
	/// @param texture 使用テクスチャ
	/// @param position 初期位置
	/// @param velocity 初期速度
	CatObject(const Texture& texture, const Vec2& position, const Vec2& velocity)
		: texture{ texture }
		, m_hitArea{ Circle{ m_ClipArea.h * m_Scale / 2 }.scaled(static_cast<double>(m_ClipArea.w) / m_ClipArea.h, 1) }
		, ClientSize{ m_ClipArea.scaledAt(m_ClipArea.pos, m_Scale).size }
		, position{ position }
		, velocity{ velocity }
	{ }

	/// @brief コピーコンストラクタ
	/// @param obj コピー元のオブジェクト
	CatObject(const CatObject &obj)
		: texture{ obj.texture }
		, m_hitArea{ obj.m_hitArea }
		, m_catData{ obj.m_catData }
		, ClientSize{ obj.ClientSize }
		, position{ position }
		, velocity{ velocity }
	{ }

	/* -- メソッド -- */

public:

	// TODO: 線形移動以外を実現するには、速度を途中で変更できればいい

	/// @brief 画面端で跳ね返るようにする
	/// @return 自分自身の参照
	CatObject& bound();

	/// @brief 画面内を設定された速度 (`velocity`) で横切る
	/// @param period 出現周期（消えた状態から現れるまでの時間）
	/// @param count 横切る回数
	/// @return 自分自身の参照
	CatObject& cross(Duration period = 0s, uint32 count = std::numeric_limits<int32>::infinity());

	/// @brief 指定した範囲内のランダムな位置に指定した周期で出現し、指定したイージング関数と時間でそれぞれフェードインアウトする
	/// @param period 出現周期（消えている時間と現れている時間）
	/// @param fadeInFunc フェードインのイージング関数
	/// @param fadeIn フェードイン時間
	/// @param fadeOutFunc フェードアウトのイージング関数
	/// @param fadeOut フェードアウト時間
	/// @param range 出現範囲
	/// @return 自分自身の参照
	CatObject& appear(Duration period, EasingFunction fadeInFunc, Duration fadeIn, EasingFunction fadeOutFunc, Duration fadeOut, const Rect &range);

	/// @brief 指定した範囲内のランダムな位置に指定した周期で出現し、線形的にそれぞれの時間でフェードインアウトする
	/// @param period 出現周期（消えている時間と現れている時間）
	/// @param fadeIn フェードイン時間
	/// @param fadeOut フェードアウト時間
	/// @param range 出現範囲
	/// @return 自分自身の参照
	CatObject& appear(Duration period, Duration fadeIn, Duration fadeOut, const Rect &range);

	/// @brief 指定した範囲内のランダムな位置に指定した周期で出現し、指定した同じイージング関数と時間でフェードインアウトする
	/// @param period 出現周期（消えている時間と現れている時間）
	/// @param fadeFunc フェードインアウトのイージング関数
	/// @param fade フェードインアウト時間
	/// @param range 出現範囲
	/// @return 自分自身の参照
	CatObject& appear(Duration period, EasingFunction fadeFunc, Duration fade, const Rect &range);

	/// @brief 指定した範囲内のランダムな位置に指定した周期で出現し、線形的に同じ時間でフェードインアウトする
	/// @param period 出現周期（消えている時間と現れている時間）
	/// @param fade フェードインアウト時間
	/// @param range 出現範囲
	/// @return 自分自身の参照
	CatObject& appear(Duration period, Duration fade, const Rect &range);

	/// @brief 自分自身が全て映る最大の範囲内のランダムな位置に指定した周期で出現し、指定したイージング関数と時間でそれぞれフェードインアウトする
	/// @param period 出現周期（消えている時間と現れている時間）
	/// @param fadeInFunc フェードインのイージング関数
	/// @param fadeIn フェードイン時間
	/// @param fadeOutFunc フェードアウトのイージング関数
	/// @param fadeOut フェードアウト時間
	/// @return 自分自身の参照
	CatObject& appear(Duration period, EasingFunction fadeInFunc, Duration fadeIn, EasingFunction fadeOutFunc, Duration fadeOut);

	/// @brief 自分自身が全て映る最大の範囲内のランダムな位置に指定した周期で出現し、線形的にそれぞれの時間でフェードインアウトする
	/// @param period 出現周期（消えている時間と現れている時間）
	/// @param fadeIn フェードイン時間
	/// @param fadeOut フェードアウト時間
	/// @return 自分自身の参照
	CatObject& appear(Duration period, Duration fadeIn, Duration fadeOut);

	/// @brief 自分自身が全て映る最大の範囲内のランダムな位置に指定した周期で出現し、指定した同じイージング関数と時間でフェードインアウトする
	/// @param period 出現周期（消えている時間と現れている時間）
	/// @param fadeFunc フェードインアウトのイージング関数
	/// @param fade フェードインアウト時間
	/// @return 自分自身の参照
	CatObject& appear(Duration period, EasingFunction fadeFunc, Duration fade);

	/// @brief 自分自身が全て映る最大の範囲内のランダムな位置に指定した周期で出現し、線形的に同じ時間でフェードインアウトする
	/// @param period 出現周期（消えている時間と現れている時間）
	/// @param fade フェードインアウト時間
	/// @return 自分自身の参照
	CatObject& appear(Duration period, Duration fade);

	/// @brief 上、右、下、左側のうちいずれかの画面端から、指定したはみだし量の位置及び時間、出現周期でランダムで出現し、退去する @n
	/// 画面端からのはみだし量は 0 を指定するとその部分からは出現しなくなる
	/// @param period 出現周期（消えている時間と現れている時間）
	/// @param inFunc 出現時の移動モーションに使うイージング関数
	/// @param in 出現にかかる時間
	/// @param outFunc 退去時の移動モーションに使うイージング関数
	/// @param out 退去にかかる時間
	/// @param overflow 画面端からのはみだし量 サイズ4の `double` 配列で、0番目が上、1番目が右、2番目が下、3番目が左 のはみだし量を意味する
	/// @return 自分自身の参照
	CatObject& appearFromEdge(Duration period, EasingFunction inFunc, Duration in, EasingFunction outFunc, Duration out, const std::array<double, 4> &overflow);

	/// @brief 上、右、下、左側のうちいずれかの画面端から、指定したはみだし量の位置及び時間、出現周期でランダムで[線形的に]出現し、退去する @n
	/// 画面端からのはみだし量は 0 を指定するとその部分からは出現しなくなる
	/// @param period 出現周期（消えている時間と現れている時間）
	/// @param in 出現にかかる時間
	/// @param out 退去にかかる時間
	/// @param overflow 画面端からのはみだし量 サイズ4の `double` 配列で、0番目が上、1番目が右、2番目が下、3番目が左 のはみだし量を意味する
	/// @return 自分自身の参照
	CatObject& appearFromEdge(Duration period, Duration in, Duration out, const std::array<double, 4> &overflow);

	/// @brief 上、右、下、左側のうちいずれかの画面端から、指定したはみだし量の位置及び時間、出現周期でランダムで出現し、[同様に]退去する @n
	/// 画面端からのはみだし量は 0 を指定するとその部分からは出現しなくなる
	/// @param period 出現周期（消えている時間と現れている時間）
	/// @param inAndOutFunc 出現、退去時の移動モーションに使うイージング関数
	/// @param inAndOut 出現、退去にかかる時間
	/// @param overflow 画面端からのはみだし量 サイズ4の `double` 配列で、0番目が上、1番目が右、2番目が下、3番目が左 のはみだし量を意味する
	/// @return 自分自身の参照
	CatObject& appearFromEdge(Duration period, EasingFunction inAndOutFunc, Duration inAndOut, const std::array<double, 4> &overflow);

	/// @brief 上、右、下、左側のうちいずれかの画面端から、指定したはみだし量の位置及び時間、出現周期でランダムで[線形的に]出現し、[同様に]退去する @n
	/// 画面端からのはみだし量は 0 を指定するとその部分からは出現しなくなる
	/// @param period 出現周期（消えている時間と現れている時間）
	/// @param inAndOut 出現、退去にかかる時間
	/// @param overflow 画面端からのはみだし量 サイズ4の `double` 配列で、0番目が上、1番目が右、2番目が下、3番目が左 のはみだし量を意味する
	/// @return 自分自身の参照
	CatObject& appearFromEdge(Duration period, Duration inAndOut, const std::array<double, 4> &overflow);

	// TODO: チャッピーとQiitaを参考に overflow~ を可変長引数で省略可能にもできるような宣言と定義をする！ -> 参考にしてもパラメータパックの意味がわからんくて無理
 	// 本当は Arg::top_, Arg::right_, Arg::bottom_, Arg::left_ を使いたかった

	/// @brief オブジェクトを描画する
	/// @return 自分自身の参照
	CatObject& draw();

	/// @brief 当たり判定領域を描画する（デバッグ用）
	/// @return 自分自身の参照
	CatObject& drawHitArea();

	/// @brief オブジェクトがタッチされたときに、現在のターゲット情報と比較して捕まえられるか試す
	/// @param target ターゲットの情報
	/// @return 自分自身の参照
	CatObject& checkCatchable(const CatData &target);
};
