#include "cat_object.hpp"

const Rect CatObject::m_ClipArea{ 0, 134, 512, 290 };

Texture CatObject::getTexture() const
{
	return m_Texture;
}

SizeF CatObject::getClientSize() const
{
	return m_ClientSize;
}

Ellipse CatObject::getHitArea() const
{
	return m_hitArea;
}

const CatData& CatObject::getCatData(CatData *out) const
{
	// 出力先変数の指定があった時のみオブジェクトをコピーして返す
	if (out)
	{
		CatData clone = m_catData;
		*out = clone;
	}
	return m_catData;
}

Rect CatObject::getMaxDisplayedArea() const
{
	// (0, 0) から 画面端から自分の長さを引いたところまでが左上基準の最大の表示領域
	return Rect{ 0, 0, Scene::Width() - static_cast<int32>(m_ClientSize.x), Scene::Height() - static_cast<int32>(m_ClientSize.y) };
}

CatObject &CatObject::setRandomVelocity(size_t level)
{
	// マジックナンバーだらけだけど仕様書に書いてあるとおり
	double min = 75.0 + (1.0 + level / 10.0);
	double max = Max(((Random(1.0, static_cast<double>(level)) / 10.0) * Max(Scene::Width(), Scene::Height()) * Math::Exp(level / Random(5, 10))), 900.0);
	velocity = RandomVec2(Random(min, max));

	return *this;
}

CatObject& CatObject::setCatData(CatData &&data)
{
	m_catData = data;
	return *this;
}

CatObject& CatObject::setAction(const LevelData::ActionData &actionData)
{
	m_actionData = actionData;
	return *this;
}

CatObject& CatObject::bound()
{
	// オブジェクトの位置を動かす
	position.moveBy(velocity * Scene::DeltaTime());

	// オブジェクトの全てが入り切る領域内で X 方向の端に到達したら
	if (const double rightEdge = Scene::Width() - m_ClientSize.x;
		x < 0 || x > rightEdge)
	{
		// 完全に画面外に出ているような状況に対しては、オブジェクト右下の点を見るようにして、
		// 右下の点が画面外にあるようなら、画面内に戻すよう速度の符号を調整する
		// (x + m_ClientSize.x) が右下の X 座標
		if ((x + m_ClientSize.x) < m_ClientSize.x || (x + m_ClientSize.x) > rightEdge)
		{
			// x <= 0 領域なら vx を正に
			// それ以外: x >= m_ClientSize.x 領域なら vx を負にして
			// 強制的に画面内に戻す
			vx = x <= 0 ? Abs(vx) : -Abs(vx);
		}
		else
		{
			// 速度を反転
			vx *= -1;
		}
	}

	// Y 方向の端に到達したら
	if (const double bottomEdge = Scene::Height() - m_ClientSize.y;
		y < 0 || y > bottomEdge)
	{
		// 以下、X 座標のときと同様
		// (y + m_ClientSize.y) が右下の Y 座標
		if (y + m_ClientSize.y < m_ClientSize.y || y + m_ClientSize.y > bottomEdge)
		{
			vy = y <= 0 ? Abs(vy) : -Abs(vy);
		}
		else
		{
			vy *= -1;
		}
	}
	return *this;
}

CatObject& CatObject::cross(Duration period, uint32 count)
{
	// 既に指定回数を上回っていて、無限回が指定されていない状況なら、これ以上処理しない
	if (m_crossData.count > count && count != std::numeric_limits<int32>::infinity())
	{
		return *this;
	}

	// 今回は Hidden と Visible の2つの状態だけ扱うようにする
	switch (m_appearanceState)
	{
		case AppearanceState::Hidden:
		{
			m_stopwatch.setInterval([&]()
				{
					// ランダムに開始位置を決める（スタート位置はこの関数で代入される）
					std::tie(m_crossData.start, m_crossData.goal) = m_changeScreenEdgePosition();

					// 増やす角度
					double angle = 0.0;

					// velocity と (start - goal) の 内積がほぼ 1 になるまで回転させ続ける
					// 内積が 1 = 角度の差が 0（長さがそれぞれ1の場合！）
					while (velocity.normalized().dot(m_crossData.difference().normalized()) < (1 - 0.0005))
					{
						velocity.rotate(++angle);
					}

					// 移動回数を増やす
					if (count != std::numeric_limits<int32>::infinity())
					{
						m_crossData.count++;
					}

					// 見える状態へ移行
					m_appearanceState = AppearanceState::Visible;
				}, period);
		}
		break;

		case AppearanceState::Visible:
		{
			// 相対する画面端に辿り着いたかどうか
			bool isReached = false;

			// 初めに出現した画面端の位置によって条件を変える
			switch (m_edgeDirection)
			{
				case ScreenEdgeDirection::Top:
				{
					// 上から始まったら左上が画面外に出るまで
					isReached = y > Scene::Height();
				}
				break;
				case ScreenEdgeDirection::Right:
				{
					// 右から始まったら右上が画面外に出るまで
					isReached = x + m_ClientSize.x < 0;
				}
				break;
				case ScreenEdgeDirection::Bottom:
				{
					// 下から始まったら左下が画面外に出るまで
					isReached = y + m_ClientSize.y < 0;
				}
				break;
				case ScreenEdgeDirection::Left:
				{
					// 左から始まったら左上が画面外に出るまで
					isReached = x > Scene::Width();
				}
				break;
			}

			// 向こう側に到着したら
			if (isReached)
			{
				// もう一度隠す
				m_appearanceState = AppearanceState::Hidden;
			}
			else
			{
				// 到着するまで動かす
				// なぜか計算上速度が逆のベクトルで出てしまったのでここで補正
				position.moveBy(-velocity * Scene::DeltaTime());
			}
		}
		break;

		default: return *this;
	}

	return *this;
}

CatObject& CatObject::appear(Duration period, EasingFunction fadeInFunc, Duration fadeIn, EasingFunction fadeOutFunc, Duration fadeOut, const Rect& range)
{
	// 外観の状態によって挙動を変える
	switch (m_appearanceState)
	{
		// 隠れている（見えない）とき
		case AppearanceState::Hidden:
		{
			m_textureAlpha = 0;

			// 出現周期だけ経過したら
			m_stopwatch.setTimeout([&]()
				{
					// 位置をランダムに変更
					position = RandomVec2(range);
					// フェードインに移行する
					m_appearanceState = AppearanceState::In;
				}, period);
		}
		break;

		// フェードインしているとき
		case AppearanceState::In:
		{
			// 手動で時間を経過させる
			m_stopwatch.forward();

			// アルファ値の増加率
			// const double increaseFactor = 1 / fadeIn.count();

			// フェードインの完了時間だけ経過したら
			if (m_stopwatch.isOver(fadeIn.count()))
			{
				// 可視状態に移行
				m_appearanceState = AppearanceState::Visible;
			}
			else
			{
				// デルタタイムと増加率の積を足し上げてフェードインさせる つもりでいたけど
				// GPTにイージング関数を使えるようにする方法を聞いたら
				// 時間を正規化してイージング関数に渡した値をそのまま使う というのが真理だったので採用
				const double t = m_stopwatch.now() / fadeIn.count();
				m_textureAlpha = Min(fadeInFunc(Min(t, 1.0)), 1.0);
			}
		}
		break;

		// 見えているとき
		case AppearanceState::Visible:
		{
			m_textureAlpha = 1;

			m_stopwatch.setTimeout([&]()
				{
					// フェードアウト状態に移行
					m_appearanceState = AppearanceState::Out;
				}, period);
		}
		break;

		case AppearanceState::Out:
		{
			// 手動で時間を経過させる
			m_stopwatch.forward();

			if (m_stopwatch.isOver(fadeOut.count()))
			{
				m_appearanceState = AppearanceState::Hidden;
			}
			else
			{
				const double t = m_stopwatch.now() / fadeOut.count();
				m_textureAlpha = Max(fadeOutFunc(Max(1.0 - t, 0.0)), 0.0);
			}
		}
		break;

		default: break;
	}

	return *this;
}

CatObject& CatObject::appear(Duration period, Duration fadeIn, Duration fadeOut, const Rect& range)
{
	return appear(period, Easing::Linear, fadeIn, Easing::Linear, fadeOut, range);
}

CatObject& CatObject::appear(Duration period, EasingFunction fadeFunc, Duration fade, const Rect& range)
{
	return appear(period, fadeFunc, fade, fadeFunc, fade, range);
}

CatObject& CatObject::appear(Duration period, Duration fade, const Rect& range)
{
	return appear(period, fade, fade, range);
}

CatObject& CatObject::appear(Duration period, EasingFunction fadeInFunc, Duration fadeIn, EasingFunction fadeOutFunc, Duration fadeOut)
{
	return appear(period, fadeInFunc, fadeIn, fadeOutFunc, fadeOut, getMaxDisplayedArea());
}

CatObject& CatObject::appear(Duration period, Duration fadeIn, Duration fadeOut)
{
	return appear(period, Easing::Linear, fadeIn, Easing::Linear, fadeOut);
}

CatObject& CatObject::appear(Duration period, EasingFunction fadeFunc, Duration fade)
{
	return appear(period, fadeFunc, fade, fadeFunc, fade);
}

CatObject& CatObject::appear(Duration period, Duration fade)
{
	return appear(period, Easing::Linear, fade, Easing::Linear, fade);
}

CatObject& CatObject::appearFromEdge(Duration period, EasingFunction inFunc, Duration in, EasingFunction outFunc, Duration out, const std::array<double, 4> &overflow)
{
	// はみだし量が全て 0 に指定されていないかどうか調べる
	bool isAllZero = true;

	for (const auto &e : overflow)
	{
		// 全ての要素で 0 かどうか調べ、前回の比較結果と AND をとる
		// 初期値を true としているので、すべて e == 0 となった場合に AND 演算で true になるようにしている
		isAllZero = (e == 0 && isAllZero);
	}

	// 全て 0 なら処理しない
	if (isAllZero)
	{
		return *this;
	}

	// このメソッドが呼び出されたらとりあえず経過時間を積算する
	m_stopwatch.forward();

	// 外観の状態によって
	switch (m_appearanceState)
	{
		// 見えていない時
		case AppearanceState::Hidden:
		{
			if (m_stopwatch.isOver(period))
			{
				// はみだし量が 0 になっている場合は出現位置を再抽選する
				do
				{
					// overflow の 0 が上
					// overflow の 1 が右
					// overflow の 2 が下
					// overflow の 3 が左 に対応（時計回り）
					m_edgeDirection = ToEnum<ScreenEdgeDirection>(Random(0, 3));
				}
				while (overflow[FromEnum(m_edgeDirection)] == 0);

				// どの端の部分が選択されたかによって
				switch (m_edgeDirection)
				{
					// 上
					case ScreenEdgeDirection::Top:
					{
						x = Random(0, getMaxDisplayedArea().w);
						y = -m_ClientSize.y;
					}
					break;

					// 右
					case ScreenEdgeDirection::Right:
					{
						x = Scene::Width();
						y = Random(0, getMaxDisplayedArea().h);
					}
					break;

					// 下
					case ScreenEdgeDirection::Bottom:
					{
						x = Random(0, getMaxDisplayedArea().w);
						y = Scene::Height();
					}
					break;

					// 左
					case ScreenEdgeDirection::Left:
					{
						x = -m_ClientSize.x;
						y = Random(0, getMaxDisplayedArea().h);
					}
					break;

					default: return *this;
				}

				m_appearanceState = AppearanceState::In;
			}
		}
		break;

		// 出現しようとしている時
		case AppearanceState::In:
		{
			if (m_stopwatch.isOver(in))
			{
				m_appearanceState = AppearanceState::Visible;
			}
			else
			{
				// 線形補間に渡すパラメータ
				// イージング関数を通すことで実質的に線形移動以外にも対応させる
				double t = Min(inFunc(m_stopwatch.now() / in.count()), 1.0);

				// どの端の部分が選択されたかによって
				switch (m_edgeDirection)
				{
					case ScreenEdgeDirection::Top:
					{
						// 位置は線形補間ではみだし量に相当する位置まで移動させる
						position = position.lerp({ x, overflow[0] }, t);
					}
					break;

					case ScreenEdgeDirection::Right:
					{
						position = position.lerp({ getMaxDisplayedArea().w - overflow[1], y}, t);
					}
					break;

					case ScreenEdgeDirection::Bottom:
					{
						position = position.lerp({ x, getMaxDisplayedArea().h - overflow[2]}, t);
					}
					break;

					case ScreenEdgeDirection::Left:
					{
						position = position.lerp({ overflow[3], y }, t);
					}
					break;

					default: return *this;
				}
			}
		}
		break;

		// 見えている時
		case AppearanceState::Visible:
		{
			if (m_stopwatch.isOver(period))
			{
				m_appearanceState = AppearanceState::Out;
			}
		}
		break;

		// 退去しようとしている時
		case AppearanceState::Out:
		{
			if (m_stopwatch.isOver(out))
			{
				m_appearanceState = AppearanceState::Hidden;
			}
			else
			{
				// パラメータ
				double t = Min(outFunc(m_stopwatch.now() / out.count()), 1.0);

				switch (m_edgeDirection)
				{
					case ScreenEdgeDirection::Top:
					{
						position = position.lerp({ x, -m_ClientSize.y }, t);
					}
					break;

					case ScreenEdgeDirection::Right:
					{
						position = position.lerp({ Scene::Width(), y }, t);
					}
					break;

					case ScreenEdgeDirection::Bottom:
					{
						position = position.lerp({ x, Scene::Height() }, t);
					}
					break;

					case ScreenEdgeDirection::Left:
					{
						position = position.lerp({ -m_ClientSize.x , y }, t);
					}
					break;

					default: return *this;
				}
			}
		}
		break;

		default: return *this;

	}

	return *this;
}

CatObject& CatObject::appearFromEdge(Duration period, Duration in, Duration out, const std::array<double, 4> &overflow)
{
	return appearFromEdge(period, Easing::Linear, in, Easing::Linear, out, overflow);
}

CatObject& CatObject::appearFromEdge(Duration period, EasingFunction inAndOutFunc, Duration inAndOut, const std::array<double, 4> &overflow)
{
	return appearFromEdge(period, inAndOutFunc, inAndOut, inAndOutFunc, inAndOut, overflow);
}

CatObject& CatObject::appearFromEdge(Duration period, Duration inAndOut, const std::array<double, 4> &overflow)
{
	return appearFromEdge(period, inAndOut, inAndOut, overflow);
}

CatObject &CatObject::act()
{
	// m_actionData.params (variant の型)に格納されている引数をもとにアクションを呼び出す
	return std::visit(InvokeAction{ *this }, m_actionData.params);
}

CatObject& CatObject::draw()
{
	// 描画前の共通処理として、当たり判定を更新
	m_hitArea.setPos(x + m_ClientSize.x / 2, y + m_ClientSize.y / 2);

	// 描画範囲をクリップ -> スケール変更 -> 任意位置にアルファ値を乗算して描画
	m_Texture(m_ClipArea).scaled(m_Scale).draw(position, ColorF{ 1.0, m_textureAlpha });
	return *this;
}

CatObject& CatObject::drawHitArea()
{
	m_hitArea.drawFrame();
	return *this;
}

bool CatObject::checkCatchable(const CatData &target, bool *const isCorrect) const
{
	*isCorrect = m_catData == target;
	return m_hitArea.leftClicked();
}

std::tuple<Vec2, Vec2> CatObject::m_changeScreenEdgePosition()
{
	Vec2 start{}, goal{};
	
	// ランダムに開始位置を決める
	switch (m_edgeDirection = ToEnum<ScreenEdgeDirection>(Random(0, 3)))
	{
		// 上側なら下側を目指す
		case ScreenEdgeDirection::Top:
		{
			start = RandomVec2(m_screenEdgeArea.top());
			goal = RandomVec2(m_screenEdgeArea.bottom());
		}
		break;
		// 右側なら左側を目指す
		case ScreenEdgeDirection::Right:
		{
			start = RandomVec2(m_screenEdgeArea.right());
			m_crossData.goal = RandomVec2(m_screenEdgeArea.left());
		}
		break;
		// 下側なら上側を目指す
		case ScreenEdgeDirection::Bottom:
		{
			start = RandomVec2(m_screenEdgeArea.bottom());
			goal = RandomVec2(m_screenEdgeArea.top());
		}
		break;
		// 左側なら右側を目指す
		case ScreenEdgeDirection::Left:
		{
			start = RandomVec2(m_screenEdgeArea.left());
			goal = RandomVec2(m_screenEdgeArea.right());
		}
		break;
	}

	position = start;

	return std::tie(start, goal);
}
