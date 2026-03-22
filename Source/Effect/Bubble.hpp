# pragma once
# include <Siv3D.hpp>

namespace UFOCat::Effect
{
	// https://siv3d.github.io/ja-jp/tutorial3/effect/#518-%E3%82%B5%E3%83%B3%E3%83%97%E3%83%AB%E6%B3%A1%E3%81%AE%E3%82%88%E3%81%86%E3%81%AA%E3%82%A8%E3%83%95%E3%82%A7%E3%82%AF%E3%83%88
	// を参考に実装しています

	/// @brief キラキラ模様の泡のエフェクト
	class Bubble : public IEffect
	{
		struct BubbleInstance
		{
			Vec2 offset;
			double startTime;
			double scale;
			ColorF color;
		};

		Vec2 m_position;

		Duration m_duration;

		Array<BubbleInstance> m_bubbles;

	public:
		Bubble(const Vec2 &position, size_t count, double range, Duration duration, double baseHue, double saturation)
			: m_position{ position }
			, m_duration{ duration }
		{
			for (int32 i = 0; i < count; ++i)
			{
				BubbleInstance bubble
				{
					.offset = RandomVec2(Circle{ range }),
					.startTime = Random(-0.3, 0.1), // 登場の時間差
					.scale = Random(0.1, 1.2),
					.color = HSV{ baseHue + Random(-30.0, 30.0) }.withS(saturation)
				};
				m_bubbles << bubble;
			}
		}

		bool update(double t) override
		{
			for (const auto& bubble : m_bubbles)
			{
				const double t2 = (bubble.startTime + t);

				if (not InRange(t2, 0.0, 1.0))
				{
					continue;
				}

				const double e = EaseOutExpo(t2);
				const double a = e * 20 * bubble.scale;

				Shape2D::Astroid((m_position + bubble.offset + (bubble.offset * 4 * t)), a, 1.2 * a)
					.draw(ColorF{ bubble.color, 0.25 })
					.drawFrame((10.0 * (1.0 - e) * bubble.scale), bubble.color);
			}

			// 1.3 秒未満ならエフェクトを継続する
			return (t < m_duration.count());
		}
	};
}
