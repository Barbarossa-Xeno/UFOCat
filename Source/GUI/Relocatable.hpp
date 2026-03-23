# pragma once
# include "Drawable.hpp"

namespace UFOCat::GUI
{
	/// @brief `Relocatable` の派生クラスに割り振るID @n
	/// 型判定を static_cast で効率的に行う目的
	/// @note 当初 dynamic_cast でやっていたが、実行速度が気になるためこの手法に切り替え
	/// こういう検証結果もある
	/// https://qiita.com/tyanmahou/items/74bd86e766eec10bab7d
	enum class RelocatableTypeID
	{
		Button, ProgressBar, TextBox
	};

	/// @brief 配置タイプ
	enum class PositionType
	{
		/// @brief 絶対座標による直接指定
		Absolute,

		/// @brief マージンによる間接指定
		Relative
	};

	/// @brief 再配置可能コンポーネント
	/// 継承先でそのクラス専用のメソッドチェーンを利用する場合、共変オーバーライドする
	class Relocatable : public Drawable
	{
	public:
		/// @brief マージン（Y 方向）
		struct Margin
		{
			double top = 0;
			double bottom = 0;
		};

	protected:
		/// @brief コンポーネントを自動整列するときに参照するマージンの値
		Margin m_margin;

		/// @brief 座標設定の初期値
		/// @note インスタンス化以降 `setPosition()` を初めて呼び出すと初期化され、それ以降変更されない
		/// `setPosition()` のオプション引数で、強制書き換え可能
		Optional<Vec2> m_initialPosition = none;

		/// @brief このコンポーネントの配置タイプ
		PositionType m_positionType = PositionType::Absolute;

	public:
		/// @brief 型ID
		/// @return 型ID (`RelocatableTypeID`)
		virtual RelocatableTypeID typeID() const noexcept = 0;

		/// @brief このコンポーネントの配置タイプ
		/// @return 配置タイプ (`PositionType`)
		inline PositionType positionType() const noexcept
		{
			return m_positionType;
		}

		/// @brief 設定したマージンを取得する
		/// @return マージン
		inline const Margin &getMargin() const noexcept
		{
			return m_margin;
		}

		/// @brief 座標設定の初期値を取得する
		/// @return 座標の初期値 まだ設定がない場合は必ず (0, 0) を返す
		inline const Vec2 &getInitialPosition() const noexcept
		{
			return m_initialPosition ? *m_initialPosition : Vec2::Zero();
		}

		// 自分自身の参照を返すため、継承先で引数の型は共変オーバーライドする
		// また、メソッドチェーンのために戻り値は普通の左辺値参照で返す

		/// @brief 描画位置に左上位置を指定する
		/// @param position 左上位置
		/// @param isOverwriteDefault 強制的に初期位置を上書きするなら true (デフォルト: false)
		/// @return 自分自身の参照
		inline virtual Relocatable &setPosition(const Vec2 &position, bool isOverwriteDefault = false) noexcept
		{
			if (not m_initialPosition or isOverwriteDefault)
			{
				m_initialPosition = position;
			}
			m_region.setPos(position);
			return *this;
		}

		/// @brief 描画位置に中央上位置を指定する
		/// @param position 中央上位置
		/// @param isOverwriteDefault 強制的に初期位置を上書きするなら true (デフォルト: false)
		/// @return 自分自身の参照
		inline virtual Relocatable &setPosition(const Arg::topCenter_<Vec2> &position, bool isOverwriteDefault = false) noexcept
		{
			m_region.setPos(position);

			// 左上以外は、一旦 RectF の指定をさせたあとにそこから取得してくる
			if (not m_initialPosition or isOverwriteDefault)
			{
				m_initialPosition = m_region.pos;
			}
			return *this;
		}

		/// @brief 描画位置に右上位置を指定する
		/// @param position 右上位置
		/// @param isOverwriteDefault 強制的に初期位置を上書きするなら true (デフォルト: false)
		/// @return 自分自身の参照
		inline virtual Relocatable &setPosition(const Arg::topRight_<Vec2> &position, bool isOverwriteDefault = false) noexcept
		{
			m_region.setPos(position);
			if (not m_initialPosition or isOverwriteDefault)
			{
				m_initialPosition = m_region.pos;
			}
			return *this;
		}

		/// @brief 描画位置に中央左位置を指定する
		/// @param position 中央左上位置
		/// @param isOverwriteDefault 強制的に初期位置を上書きするなら true (デフォルト: false)
		/// @return 自分自身の参照
		inline virtual Relocatable &setPosition(const Arg::leftCenter_<Vec2> &position, bool isOverwriteDefault = false) noexcept
		{
			m_region.setPos(position);
			if (not m_initialPosition or isOverwriteDefault)
			{
				m_initialPosition = m_region.pos;
			}
			return *this;
		}

		/// @brief 描画位置に中央右位置を指定する
		/// @param position 中央右位置
		/// @param isOverwriteDefault 強制的に初期位置を上書きするなら true (デフォルト: false)
		/// @return 自分自身の参照
		inline virtual Relocatable &setPosition(const Arg::rightCenter_<Vec2> &position, bool isOverwriteDefault = false) noexcept
		{
			m_region.setPos(position);
			if (not m_initialPosition or isOverwriteDefault)
			{
				m_initialPosition = m_region.pos;
			}
			return *this;
		}

		/// @brief 描画位置に左下位置を指定する
		/// @param position 左下位置
		/// @param isOverwriteDefault 強制的に初期位置を上書きするなら true (デフォルト: false)
		/// @return 自分自身の参照
		inline virtual Relocatable &setPosition(const Arg::bottomLeft_<Vec2> &position, bool isOverwriteDefault = false) noexcept
		{
			m_region.setPos(position);
			if (not m_initialPosition or isOverwriteDefault)
			{
				m_initialPosition = m_region.pos;
			}
			return *this;
		}

		/// @brief 描画位置に中央下位置を指定する
		/// @param position 中央下位置
		/// @param isOverwriteDefault 強制的に初期位置を上書きするなら true (デフォルト: false)
		/// @return 自分自身の参照
		inline virtual Relocatable &setPosition(const Arg::bottomCenter_<Vec2> &position, bool isOverwriteDefault = false) noexcept
		{
			m_region.setPos(position);
			if (not m_initialPosition or isOverwriteDefault)
			{
				m_initialPosition = m_region.pos;
			}
			return *this;
		}

		/// @brief 描画位置に右下位置を指定しボタンを描画する
		/// @param position 右下位置
		///	@param isOverwriteDefault 強制的に初期位置を上書きするなら true (デフォルト: false)
		/// @return 自分自身の参照
		inline virtual Relocatable &setPosition(const Arg::bottomRight_<Vec2> &position, bool isOverwriteDefault = false) noexcept
		{
			m_region.setPos(position);
			if (not m_initialPosition or isOverwriteDefault)
			{
				m_initialPosition = m_region.pos;
			}
			return *this;
		}

		/// @brief 描画位置に中央位置を指定する
		/// @param position 中央位置
		///	@param isOverwriteDefault 強制的に初期位置を上書きするなら true (デフォルト: false)
		/// @return 自分自身の参照
		inline virtual Relocatable &setPositionAt(const Vec2 &position, bool isOverwriteDefault = false) noexcept
		{
			m_region.setPos(Arg::center = position);
			if (not m_initialPosition or isOverwriteDefault)
			{
				m_initialPosition = m_region.pos;
			}
			return *this;
		}

		/// @brief マージンを設定する
		/// @param margin マージン
		/// @return 自分自身の参照
		inline virtual Relocatable &setMargin(const Margin &margin) noexcept
		{
			m_margin = margin;
			return *this;
		}
	};
	// ほんとは CRTP（奇妙に再帰したテンプレートパターン）を使ってオーバーライドを避けたかったけど
	// それをやめるとコンパイル時点で継承元があいまいになって
	// CRTP 先のクラスを 基底に入れられない問題が起きてしまった
	// しかし CRTP 先のクラスはテンプレート引数が必要なのでそれなしで実体化不可能
	// 詰んだので共変オーバーライドをいちいちする方式にした
}
