# pragma once

namespace UFOCat::GUI
{
	/// @brief 描画可能コンポーネントの抽象クラス
	class Drawable
	{
	protected:
		/// @brief この GUI コンポーネントの画面上の領域
		RectF m_region;

	public:
		virtual ~Drawable() = default;

		/// @brief この GUI コンポーネントの画面上の領域を取得する
		/// @return 領域 (RectF)
		const RectF &getRegion() const noexcept
		{
			return m_region;
		}

		/// @brief 描画する
		virtual void draw() const = 0;
	};
}
