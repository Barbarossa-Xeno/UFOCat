#pragma once

namespace UFOCat::Util
{
	class DropShadowRT
	{
		// シーン全体を白色で透明なレンダーテクスチャで覆う
		const RenderTexture m_ShadowTexture{ Scene::Size(), ColorF{ 1.0, 0.0 } };

		// 1 / 4 にダウンサンプリングする、ブラー用のレンダーテクスチャ
		const RenderTexture m_Blur4{ m_ShadowTexture.size() / 4 };

		// ブラー用の中間テクスチャ
		const RenderTexture m_Internal4{ m_ShadowTexture.size() / 4 };

	public:

		RectF draw(const TextureRegion& targetTexture, ColorF color, Vec2 position, Vec2 offset, double scale)
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
				targetTexture.draw(position, ColorF{1.0, color.a});
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
