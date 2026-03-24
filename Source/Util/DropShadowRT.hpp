#pragma once

namespace UFOCat::Util
{
	/// @brief ドロップシャドウのレンダーテクスチャを用意するクラス
	/// `draw()` で実際に描画する
	/// @note https://siv3d.github.io/ja-jp/tutorial3/render-texture/?h=%E5%BD%B1#5210-%E4%BB%BB%E6%84%8F%E5%BD%A2%E7%8A%B6%E3%81%AE%E3%82%B7%E3%83%A3%E3%83%89%E3%82%A6
	class DropShadowRT
	{
		// シーン全体を白色で透明なレンダーテクスチャで覆う
		const RenderTexture m_ShadowTexture{ Scene::Size(), ColorF{ 1.0, 0.0 } };

		// 1 / 4 にダウンサンプリングする、ブラー用のレンダーテクスチャ
		const RenderTexture m_Blur4{ m_ShadowTexture.size() / 4 };

		// ブラー用の中間テクスチャ
		const RenderTexture m_Internal4{ m_ShadowTexture.size() / 4 };

	public:
		/// @brief ScopedRenderTarget をつくり、対象のテクスチャにドロップシャドウを描画する
		/// @param targetTexture 対象テクスチャ
		/// @param color 影の色
		/// @param origin 影を描画する基準の中心位置（上下左右からの中心位置）
		/// @param offset 影を描画するうえでの中心位置からのずれ
		/// @param scale 影の大きさ
		/// @return 影の描画領域
		RectF draw(const TextureRegion &targetTexture, ColorF color, Vec2 origin, Vec2 offset, double scale) const
		{
			// 影の形状を描く
			{
				// レンダーターゲットを白色透明で初期化
				const ScopedRenderTarget2D target{ m_ShadowTexture.clear(ColorF{ 1.0, 0.0 }) };

				// RGB 値は無視して、描画された最大のアルファ値を保持するブレンドステートを適用することで
				// 透明部分以外を取る
				const ScopedRenderStates2D blend{ BlendState::MaxAlpha };

				// 影を任意方向に落とすため、描画位置をずらす
				const Transformer2D transform{ Mat3x2::Translate(offset.x, offset.y) };

				// 実テクスチャに対する任意スケールで現在の透明度を反映して描画
				targetTexture.scaled(scale).drawAt(origin, ColorF{ 1.0, color.a });
			}

			// ShadowTexture をダウンサンプリング + ガウスぼかし
			{
				Shader::Downsample(m_ShadowTexture, m_Blur4);
				Shader::GaussianBlur(m_Blur4, m_Internal4, m_Blur4);
			}

			// ぼかした影を描く
			return m_Blur4.resized(Scene::Size()).draw(color);
		}
	};
}
