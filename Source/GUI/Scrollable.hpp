# pragma once
# include "ScrollView.hpp"

namespace UFOCat::GUI
{
	/// @brief 派生クラスにスクロール機能を提供する CRTP 基底クラス
	/// @tparam Derived 派生クラス
	template <typename Derived>
	class Scrollable
	{
	protected:

		/// @brief スクロール可能部分をつくるコンテナ
		ScrollView m_container;

	public:

		/// @brief コンテナ内に配置するコンテンツを追加する
		/// @tparam ...TContents `Relocatable` なコンポーネント（パラメータパック）
		/// @param ...contents `Relocatable` なコンポーネントを複数指定（可変長引数）
		/// @return 自分自身の参照
		template <std::derived_from<Relocatable> ...TContents>
		inline Derived &addContents(const TContents &...contents)
		{
			m_container.addContents(contents...);
			return static_cast<Derived&>(*this);
		}

		/// @brief コンテナ内に配置するコンテンツを設定しなおす
		/// @tparam ...TContents `Relocatable` なコンポーネント（パラメータパック）
		/// @param ...contents `Relocatable` なコンポーネントを複数指定（可変長引数）
		/// @return 自分自身の参照
		template <std::derived_from<Relocatable> ...TContents>
		inline Derived &setContents(const TContents &...contents)
		{
			m_container.setContents(contents...);
			return static_cast<Derived&>(*this);
		}
	};
}
