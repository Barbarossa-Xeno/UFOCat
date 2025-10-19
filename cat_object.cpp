#include "cat_object.hpp"

const Rect CatObject::m_ClipArea{ 0, 134, 512, 290 };

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
	return Rect{ 0, 0, Scene::Width() - static_cast<int32>(ClientSize.x), Scene::Height() - static_cast<int32>(ClientSize.y) };
}

CatObject& CatObject::setCatData(CatData &&data)
{
	m_catData = data;
	return *this;
}

CatObject& CatObject::setAction(Phase::ActionData &actionData)
{
	m_actionData = actionData;
	return *this;
}

CatObject& CatObject::bound()
{
	// オブジェクトの位置を動かす
	position.moveBy(velocity * Scene::DeltaTime());

	// オブジェクトの全てが入り切る領域内で X 方向の端に到達したら
	if (const double rightEdge = Scene::Width() - ClientSize.x;
		x < 0 || x > rightEdge)
	{
		// 完全に画面外に出ているような状況に対しては、オブジェクト右下の点を見るようにして、
		// 右下の点が画面外にあるようなら、画面内に戻すよう速度の符号を調整する
		// (x + ClientSize.x) が右下の X 座標
		if ((x + ClientSize.x) < ClientSize.x || (x + ClientSize.x) > rightEdge)
		{
			// x <= 0 領域なら vx を正に
			// それ以外: x >= ClientSize.x 領域なら vx を負にして
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
	if (const double bottomEdge = Scene::Height() - ClientSize.y;
		y < 0 || y > bottomEdge)
	{
		// 以下、X 座標のときと同様
		// (y + ClientSize.y) が右下の Y 座標
		if (y + ClientSize.y < ClientSize.y || y + ClientSize.y > bottomEdge)
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
		case Appearance::Hidden:
		{
			// 隠れている時のみ、タイマーを測る
			m_stopwatch.forward();

			if (m_stopwatch.isOver(period))
			{
				// この Rect から スクリーンの Rect を切り抜いたエリア内（画面外の部分）をスタートまたはゴールとする
				const RectF goalArea = { -Vec2(ClientSize), Scene::Width() + ClientSize.x, Scene::Height() + ClientSize.y };

				// ランダムに開始位置を決める
				switch (m_edgeState = ToEnum<ScreenEdge>(Random(0, 3)))
				{
					// 上側なら下側を目指す
				case ScreenEdge::Top:
				{
					m_crossData.start = RandomVec2(goalArea.top());
					m_crossData.goal = RandomVec2(goalArea.bottom());
				}
				break;
				// 右側なら左側を目指す
				case ScreenEdge::Right:
				{
					m_crossData.start = RandomVec2(goalArea.right());
					m_crossData.goal = RandomVec2(goalArea.left());
				}
				break;
				// 下側なら上側を目指す
				case ScreenEdge::Bottom:
				{
					m_crossData.start = RandomVec2(goalArea.bottom());
					m_crossData.goal = RandomVec2(goalArea.top());
				}
				break;
				// 左側なら右側を目指す
				case ScreenEdge::Left:
				{
					m_crossData.start = RandomVec2(goalArea.left());
					m_crossData.goal = RandomVec2(goalArea.right());
				}
				break;
				default: return *this;
				}

				// 増やす角度
				double angle = 0.0;

				// velocity と (start - goal) の 内積がほぼ 1 になるまで回転させ続ける
				// 内積が 1 = 角度の差が 0（長さがそれぞれ1の場合！）
				while (velocity.normalized().dot(m_crossData.difference().normalized()) < (1 - 0.0005))
				{
					velocity.rotate(++angle);
				}

				// スタート地点へ移動
				position = m_crossData.start;

				// 移動回数を増やす
				if (count != std::numeric_limits<int32>::infinity())
				{
					m_crossData.count++;
				}

				// 見える状態へ移行
				m_appearanceState = Appearance::Visible;
			}
			
		}
		break;

		case Appearance::Visible:
		{
			// 相対する画面端に辿り着いたかどうか
			bool isReached = false;

			// 初めに出現した画面端の位置によって条件を変える
			switch (m_edgeState)
			{
				case ScreenEdge::Top:
				{
					// 上から始まったら左上が画面外に出るまで
					isReached = y > Scene::Height();
				}
				break;
				case ScreenEdge::Right:
				{
					// 右から始まったら右上が画面外に出るまで
					isReached = x + ClientSize.x < 0;
				}
				break;
				case ScreenEdge::Bottom:
				{
					// 下から始まったら左下が画面外に出るまで
					isReached = y + ClientSize.y < 0;
				}
				break;
				case ScreenEdge::Left:
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
				m_appearanceState = Appearance::Hidden;
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
	// このメソッドが呼び出されたらとりあえず経過時間を積算する
	m_stopwatch.forward();

	// 外観の状態によって挙動を変える
	switch (m_appearanceState)
	{
		// 隠れている（見えない）とき
		case Appearance::Hidden:
		{
			m_textureAlpha = 0;

			// 出現周期だけの時間が経過したら
			if (m_stopwatch.isOver(period))
			{
				// 位置をランダムに変更
				position = RandomVec2(range);
				// フェードインに移行する
				m_appearanceState = Appearance::In;
			}
		}
		break;

		// フェードインしているとき
		case Appearance::In:
		{
			// アルファ値の増加率
			// const double increaseFactor = 1 / fadeIn.count();

			// フェードインの完了時間だけ経過したら
			if (m_stopwatch.isOver(fadeIn.count()))
			{
				// 可視状態に移行
				m_stopwatch.reset(fadeIn.count());
				m_appearanceState = Appearance::Visible;
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
		case Appearance::Visible:
		{
			m_textureAlpha = 1;

			if (m_stopwatch.isOver(period.count()))
			{
				m_stopwatch.reset(period.count());

				// フェードアウト状態に移行
				m_appearanceState = Appearance::Out;
			}
		}
		break;

		case Appearance::Out:
		{
			if (m_stopwatch.isOver(fadeOut.count()))
			{
				m_stopwatch.reset(fadeOut.count());
				m_appearanceState = Appearance::Hidden;
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
		case Appearance::Hidden:
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
					m_edgeState = ToEnum<ScreenEdge>(Random(0, 3));
				}
				while (overflow[FromEnum(m_edgeState)] == 0);

				// どの端の部分が選択されたかによって
				switch (m_edgeState)
				{
					// 上
					case ScreenEdge::Top:
					{
						x = Random(0, getMaxDisplayedArea().w);
						y = -ClientSize.y;
					}
					break;

					// 右
					case ScreenEdge::Right:
					{
						x = Scene::Width();
						y = Random(0, getMaxDisplayedArea().h);
					}
					break;

					// 下
					case ScreenEdge::Bottom:
					{
						x = Random(0, getMaxDisplayedArea().w);
						y = Scene::Height();
					}
					break;

					// 左
					case ScreenEdge::Left:
					{
						x = -ClientSize.x;
						y = Random(0, getMaxDisplayedArea().h);
					}
					break;

					default: return *this;
				}

				m_appearanceState = Appearance::In;
			}
		}
		break;

		// 出現しようとしている時
		case Appearance::In:
		{
			if (m_stopwatch.isOver(in))
			{
				m_appearanceState = Appearance::Visible;
			}
			else
			{
				// 線形補間に渡すパラメータ
				// イージング関数を通すことで実質的に線形移動以外にも対応させる
				double t = Min(inFunc(m_stopwatch.now() / in.count()), 1.0);

				// どの端の部分が選択されたかによって
				switch (m_edgeState)
				{
					case ScreenEdge::Top:
					{
						// 位置は線形補間ではみだし量に相当する位置まで移動させる
						position = position.lerp({ x, overflow[0] }, t);
					}
					break;

					case ScreenEdge::Right:
					{
						position = position.lerp({ getMaxDisplayedArea().w - overflow[1], y}, t);
					}
					break;

					case ScreenEdge::Bottom:
					{
						position = position.lerp({ x, getMaxDisplayedArea().h - overflow[2]}, t);
					}
					break;

					case ScreenEdge::Left:
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
		case Appearance::Visible:
		{
			if (m_stopwatch.isOver(period))
			{
				m_appearanceState = Appearance::Out;
			}
		}
		break;

		// 退去しようとしている時
		case Appearance::Out:
		{
			if (m_stopwatch.isOver(out))
			{
				m_appearanceState = Appearance::Hidden;
			}
			else
			{
				// パラメータ
				double t = Min(outFunc(m_stopwatch.now() / out.count()), 1.0);

				switch (m_edgeState)
				{
					case ScreenEdge::Top:
					{
						position = position.lerp({ x, -ClientSize.y }, t);
					}
					break;

					case ScreenEdge::Right:
					{
						position = position.lerp({ Scene::Width(), y }, t);
					}
					break;

					case ScreenEdge::Bottom:
					{
						position = position.lerp({ x, Scene::Height() }, t);
					}
					break;

					case ScreenEdge::Left:
					{
						position = position.lerp({ -ClientSize.x , y }, t);
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
	return std::visit(Invoke{ *this }, m_actionData.params);
}

CatObject& CatObject::draw()
{
	// 描画前の共通処理として、当たり判定を更新
	m_hitArea.setPos(x + ClientSize.x / 2, y + ClientSize.y / 2);

	// 描画範囲をクリップ -> スケール変更 -> 任意位置にアルファ値を乗算して描画
	texture(m_ClipArea).scaled(m_Scale).draw(position, ColorF{ 1.0, m_textureAlpha });
	return *this;
}

CatObject& CatObject::drawHitArea()
{
	m_hitArea.drawFrame();
	return *this;
}

CatObject& CatObject::checkCatchable(const CatData &target)
{
	if (m_hitArea.leftClicked())
	{
		if (m_catData == target)
		{
			Console << U"合ってる";
		}
		else
		{
			Console << U"別人だわ";
		}
	}

	return *this;
}
