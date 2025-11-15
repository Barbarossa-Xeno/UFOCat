# pragma once
# include "Common.hpp"

namespace UFOCat
{
	/// @brief ターゲットを知らせるシーン
	class Wanted : public App::Scene
	{
	private:

		/// @brief ターゲットの情報
		std::shared_ptr<CatObject> m_target = nullptr;

		/// @brief GUI 要素
		struct
		{
			/// @brief 現在のレベルを表現するバー
			Component::GUI::ProgressBar levelBar;

			/// @brief 「UFO猫を探しています」のチラシのテクスチャ
			Texture flyer;
		}
		m_gui;

		/// @brief ターゲット猫の毛色データを、色アイコンと色名で表示するためのコンポーネント
		/// @param name 色名
		/// @param color 色
		/// @param leftCenter 左中央を基準としたの配置位置  
		/// @param size 表示サイズ（色アイコンの半径に相当する）
		/// @return アイコンとマージン、色名の表示領域全てを足した範囲を RectF で返す
		RectF m_showColorData(String name, Color color, Vec2 leftCenter, double size) const;

	public:

		Wanted(const InitData &init);

		void update() override;

		void draw() const override;

	};
}
