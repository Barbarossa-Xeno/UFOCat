# include "Level.hpp"

namespace UFOCat
{
	LevelData& Level::m_currentLevel() const
	{
		// 現在のレベル番号で共有データのレベルデータを参照して返す
		return getData().levels[getData().levelIndex];
	}

	Array<Score::LevelRecord> &Level::m_currentRecords() const
	{
		// 現在のレベルのスコア記録は、共有データのスコア記録のリストの
		// 最後の要素の records で管理されているので、そこを参照して返す
		return getData().scores.back().records;
	}

	void Level::m_setTargetSpawnTime(size_t level)
	{
		// 定式の詳細は仕様書参照
		const double &&term1 = m_currentLevel().timeLimit.count() / Random(2, 4);
		const double &&term2 = level * (m_currentLevel().breedData.total() * m_currentLevel().intervalData.period.count()) / (m_currentLevel().actionDataList.size() * m_currentLevel().breedData.similar);

		// 一度、ターゲットスポーン時間を設定
		m_targetAppearTime = Duration{ Min((term1 + term2), 0.75 * m_currentLevel().timeLimit.count()) };
	}

	bool Level::m_hasAppearedTarget() const
	{
		if (not getData().spawns.isEmpty())
		{
			// ターゲットは 0番目 に入れるよう保証しているので、
			// 0番目が nullptr でなければ出現済みとする
			return getData().spawns[0] != nullptr;
		}
		else
		{
			return false;
		}
	}

	bool Level::m_isAvailableNextLevel() const
	{
		// 次のレベルのインデックスがレベルの数未満であれば進める
		return (getData().levelIndex + 1) < getData().levels.size();
	}

	Level::Level(const InitData& init)
		: IScene{ init }
	{
		// GUI初期化 と テクスチャ取得
		{
			// タイマーのテクスチャ ちなみに針はなくてプログラム側で動的に描画する
			m_gui.timer = Texture{ Resource(U"texture/timer.png"), TextureDesc::Mipped };

			// ダイアログのアラート内容を設定
			m_gui.quitAlert.setContents
			(
				GUI::TextBox{ FontAsset(Util::FontName::YuseiMagic)(U"本当に戻りますか？\nここまでのデータは失われます"), 20, Util::Palette::Brown }.setPositionAt({ 135, 40 })
			).setSize({ 350, 200 });

			m_bg = getData().backgrounds.choice();
		}

		// 前回レベルでスポーンした猫を吹っ飛ばし、unique_ptr も解放する
		getData().spawns.release();

		// 0 番目に空のポインタを入れて置き、
		// ターゲットをスポーンさせるときはここを上書きする形にする
		// こうすることで、0 番目が nullptr かどうかでターゲットの出現判定ができるようになるうえに、
		// `catchable()` で判定をする際に、ターゲットを最も初めにチェックするので、
		// 当たり判定を優遇することができる（ミスタップを起こしにくい）
		// しかも一番初めに描画されるので、ターゲットが他の猫に隠れて見えづらいパターンが発生することもあり、難易度がちょっと上がる
		// （裏側にいたとしても判定は優先される）
		getData().spawns << nullptr;

		// 前回レベルで選んだ猫を吹っ飛ばし、shared_ptr も解放する
		m_selections.release();

		// 先にスコアのレベル情報を設定
		m_score.level = getData().levelIndex + 1;

		// 前回シーンで決めたターゲットを取得
		m_target = getData().cats[getData().targetId];

		// レベルデータのうち、登場する猫の数に関するデータを取得する
		const auto& similarCount = m_currentLevel().breedData.similar;
		const auto& otherCount = m_currentLevel().breedData.other;

		// それぞれ、似ている猫とそうじゃない猫を絞り込む用の配列を用意する
		Array<const CatData*> similars, others;

		// 全種類の猫の中から、ターゲットと似ている猫とそうでない猫を振り分ける
		for (const auto &cat : getData().cats)
		{
			// それと、ターゲットと同じのを参照しないように保証する
			if (m_currentLevel().similarity == cat->getSameDataCount(*m_target)
				and *cat != *m_target)
			{
				similars << cat.get();
			}
			// 類似条件を下回るのを「その他」としてカウント
			else if (m_currentLevel().similarity > cat->getSameDataCount(*m_target))
			{
				others << cat.get();
			}
		}

		// 宇宙船演算子を使ってみる
		// 上で絞り込んだ結果、similars の数が既定 (similarCount) より多すぎるか、少なすぎる場合に調整する
		// まずは多すぎる場合
		if (auto comp = similars.size() <=> similarCount;
			comp > 0)
		{
			// 多すぎる場合はランダムに頭数を削る
			similars.shuffle().resize(similarCount);
		}
		// 少なすぎる場合
		else if (comp < 0)
		{
			// 条件を緩和して絞り込んだ猫を入れる配列
			Array<const CatData*> looses;

			// 類似度を1つ下げて絞り込んだものを looses に入れる
			for (const auto &cat : others)
			{
				if ((m_currentLevel().similarity - 1) == cat->getSameDataCount(*m_target)
					and *cat != *m_target)
				{
					looses << cat;
				}
			}

			// 足りなかった分を looses か others から補う
			// 絞り込めた猫が既定の数に達するまで繰り返す
			while (similars.size() < similarCount)
			{
				if (not looses.isEmpty())
				{
					similars << looses.shuffle().choice();
				}
				else
				{
					// others から補うことになった場合は補ったやつを元のリストから消す
					// others はもともと多く絞り込まれるはずなので、枯渇することはない
					const auto &instead = others.shuffle().choice();
					similars << instead;
					others.remove(instead);
				}
			}

		}

		// others は既定の数から溢れている可能性が高いので、ランダムに削る
		if (others.size() > otherCount)
		{
			others.shuffle().resize(otherCount);
		}
		// 既定の数に達しない場合は、それでもよしとする
		// 無理にほかの種類も含めようとすると、難易度が上がりすぎる可能性がある？

		// 結局、似てるのと他のでどの猫を使うのかまとめておく
		// そのためにまずは id を使って重複を消す
		Array<size_t> ids = similars.map([](const auto &s) { return s->id; }).append(others.map([](const auto &o) { return o->id; } ));

		// 重複無しにするときは set が効率的
		m_selectionsId = HashSet<size_t>{ ids.begin(), ids.end() };

		
		for (auto &&id : m_selectionsId)
		{
			// 必要なテクスチャアセットの非同期ロードを始める
			TextureAsset::LoadAsync(Cat(id));

			// 猫データも共有しておく
			m_selections << getData().cats[id];
		}
		// ロードの間も以下を続行

		// レベル中に行うアクションリストの中から、それぞれの発生確率だけを抜き取ったリストで確率分布をつくる
		m_actionProbabilities = DiscreteDistribution{ m_currentLevel().actionDataList.map([](const LevelData::ActionData& data) { return data.probability; }) };

		// 現在のレベルに合わせてターゲットの出現時刻を設定
		m_setTargetSpawnTime(getData().levelIndex + 1);

		AudioAsset(getData().bgmName).stop();

		// BGM 抽選
		getData().bgmName = Array{ Util::AudioName::BGM::Level01, Util::AudioName::BGM::Level02 }.choice();

		// 3、2、1、GO! のカウントダウンを入れるための待機時間をセット
		// シーンのフェードインアウト時間を考慮して少し長め = 4s に取る
		getData().timer.pause();
		getData().timer.set(Duration{ m_prevTimerRemaining });
	}

	void Level::update()
	{
		// # ステート依存処理
		switch (m_state)
		{
			// ## プレイ前: タイマーでタイミングを計って、音を鳴らしたりプレイ開始したりする
			case State::Before:
			{
				// タイマー稼働してない
				if (not getData().timer.isRunning())
				{
					// 停止していて、残り時間が0でない場合は、
					// 初期化時にタイマーをセットしたときの続きということなので
					if (not getData().timer.reachedZero())
					{
						// 使用する猫（選ばれたもの + ターゲット）のアセットが全てロードされていれば
						if (std::all_of(m_selectionsId.begin(), m_selectionsId.end(), [](const size_t id)
							{
								return TextureAsset::IsReady(Cat(id));
							}) and TextureAsset::IsReady(Cat(m_target->id)))
						{
							// カウントダウンはじめ
							getData().timer.start();
						}
					}
					// 停止していて、かつ残り時間が 0 になっている場合は、
					// 1つ前の if 文で始めたカウントダウンが終わったということなので
					else
					{
						// ステートをプレイ中に変更する
						m_state = State::Playing;

						// 制限時間を決めて、タイマー開始
						// 1.75s 猶予を持たせて、BGM再生までの尺とする
						getData().timer.restart(m_currentLevel().timeLimit + 1.75s);
					}
				}
				// タイマー稼働してる = プレイ前のカウントダウン中
				else
				{
					// この else ステートメントの最後で、m_prevTimerRemaining を timer.s() で更新している
					// この timer.s() は常に整数秒を返すため、m_prevTimerRemaining より小さくなったかどうかを更新前に判定して
					// 整数秒が変化したタイミングを検知できる

					// タイマーの残り秒数が前回の更新から減っていたら、カウントダウンの音を鳴らす
					if (m_prevTimerRemaining > getData().timer.s())
					{
						if (getData().timer.s() > 0)
						{
							// カウントダウンの音
							// ここでは 3 回なるはず（m_prevTimerRemaining が最初 4 なので 3, 2, 1 のタイミング）
							AudioAsset(Util::AudioName::SE::CountDown).playOneShot();
						}
						else
						{
							// 残り時間 0 の瞬間でスタートの音（ぴーっ）
							AudioAsset(Util::AudioName::SE::StartLevel).playOneShot();
						}
					}

					// 現在の timer.s() で更新しておく
					m_prevTimerRemaining = getData().timer.s();
				}

				break;
			}
			// ## プレイ中: スポーン処理とタップや制限時間での終了処理
			case State::Playing:
			{
				// ### スポーン処理　出現ペースに応じた間隔
				m_watch.setInterval([this]()
				{
					// ターゲットの出現時刻を超えていて、ターゲットがまだ出現していなかったら
					if (getData().timer.remaining() <= m_targetAppearTime and (not m_hasAppearedTarget()))
					{
						// ターゲットを湧かせる
						getData().spawns[0] = std::make_unique<CatObject>(CatObject{ TextureAsset(Cat(m_target->id)) }.setCatData(*m_target));

						// ターゲットにも同様にアクションと速度の設定を行う
						getData().spawns[0]->setAction(DiscreteSample(m_currentLevel().actionDataList, m_actionProbabilities)).setRandomVelocity(getData().levelIndex + 1);
					}
					else
					{
						// ターゲット以外の猫をランダムに出現ペースの分だけ選んで（コピーして）追加
						for (uint32 i = 0; i < m_currentLevel().intervalData.count; i++)
						{
							// 1個適当に選ぶ
							const auto &selection = m_selections.choice();

							// アクションを抽選してセットし、現在のレベルに合わせて速度もランダムに決める
							// そしてスポーンリストに追加
							getData().spawns << std::make_unique<CatObject>
												(
													CatObject{ TextureAsset(Cat(selection->id)) }
														.setCatData(*selection)
														.setAction(DiscreteSample(m_currentLevel().actionDataList, m_actionProbabilities))
														.setRandomVelocity(getData().levelIndex + 1)
												);
						}
					}
				}, m_currentLevel().intervalData.period);

				// ### 制限時間内と時間超過後での処理
				
				// #### 制限時間内
				if (getData().timer.isRunning())
				{
					// タイマー残り時間が制限時間を超えている場合は、まだプレイできないので、以降の処理をしないで抜ける
					// つまり State::Before の最後で尺を作った 1.75s の間は、ターゲットが出現していてもタップしても反応しないようにする
					if (getData().timer.s() > m_currentLevel().timeLimit.count())
					{
						return;
					}
					// これ以下の処理ではタイマー残り時間が 30s 以下であることを保証する

					AudioAsset(getData().bgmName).play();

					// ターゲットが初めて画面上に見えたかどうかを記録する
					if (getData().spawns[0] and (not m_targetFirstVisible))
					{
						m_targetFirstVisible = getData().spawns[0]->isVisible();

						// 初めて見えた時点での残り時間に変更しておく
						m_targetAppearTime = getData().timer.remaining();
					}

					// ターゲットから順に捜査する
					for (const auto &cat : getData().spawns)
					{
						if (not cat)
						{
							continue;
						}

						// 猫をタッチしたら、その正誤を代入
						if (cat->act().checkCatchable(*m_target, &m_score.isCorrect))
						{
							// 捕まえた猫を記録
							m_caught = &cat;

							// ターゲットとの正誤にかかわらず、触ったことにはしておく
							m_score.isCaught = true;

							// 反応時間を記録
							m_score.response = (m_targetAppearTime - getData().timer.remaining()).count();

							// 連続正解数を記録

							// 仮変数
							uint32 temp_consecutive = 0;

							// 連続正解数の計算
							for (size_t i = 0; i < m_currentRecords().size() - 1; i++)
							{
								// 次のスコアデータが存在しない場合はこの for ループ終了
								if (not m_currentRecords()[i + 1].level)
								{
									break;
								}

								// 今のレベルと次のレベルの両方で正解していたら増やす
								if (m_currentRecords()[i].isCorrect and m_currentRecords()[i + 1].isCorrect)
								{
									++temp_consecutive;
								}
								else
								{
									temp_consecutive = 0;
								}
							}

							// 記録
							m_score.consecutiveCorrect = temp_consecutive;

							// プレイ終了へ
							m_state = State::Finish;

							// 明示的に内部ストップウォッチリセット（でないと積算時間が持ち越される）
							m_watch.reset();

							AudioAsset(Util::AudioName::SE::FinishLevel).playOneShot();
							AudioAsset(getData().bgmName).fadeVolume(0.0, 1s);

							// switch を抜ける
							break;
						}
					}
				}
				// #### 時間外
				else
				{
					// タイマーがセットだけされている状態 -> 前のステートからの遷移直後
					if (not getData().timer.reachedZero())
					{
						// なのでタイマースタート
						getData().timer.start();
					}
					else
					{
						// 時間切れでプレイ終了するときは、猫も捕まえられていないので
						// スコア計算などをしない

						// 制限時間が終わったら、終了表示を出しに行く (draw 参照)
						m_state = State::Finish;

						// 明示的にストップウォッチリセット（でないと積算時間が持ち越される）
						m_watch.reset();

						AudioAsset(Util::AudioName::SE::FinishLevel).playOneShot();
						AudioAsset(getData().bgmName).fadeVolume(0.0, 1s);
					}
				}

				break;
			}
			// ## プレイ後: 3s 待ってから結果表示へ
			case State::Finish:
			{
				// 3s 経ったらレベル終わり画面を出しに行く
				m_watch.setTimeout([this]()
				{
					m_state = State::After;

					// 明示的に内部ストップウォッチリセット（でないと積算時間が持ち越される）
					m_watch.reset();

					AudioAsset(getData().bgmName).stop();
				}, 3s);

				break;
			}
			// ## 結果表示
			case State::After:
			{
				// 0.1s 待って音が鳴る
				m_watch.setTimeout([this]()
				{
					// 「SEが鳴っている間」を取得したいのでふつうの play を使う
					// 捕まえられなかったら時間切れの音
					// 捕まえられたけど正解していなかったら不正解の音
					// 捕まえられたうえで正解していたら正解の音
					AudioAsset(m_score.isCaught ? (m_score.isCorrect ? Util::AudioName::SE::Correct : Util::AudioName::SE::Incorrect) : Util::AudioName::SE::TimeUp).play();
				}, 0.1s);

				// # GUI 処理
				{
					// 次のレベルへ進めるかは、今回が合っていて かつ 次のレベルが存在する必要がある
					bool canContinue = (m_score.isCorrect and m_isAvailableNextLevel());

					// 次のレベルへ進むボタン
					if (m_gui.toNextLevel.set(32, U"次のレベルへ", GUI::LayoutMode::Absolute, canContinue)
										 .setPosition(Arg::bottomRight = (Scene::Size() - Vec2{ 10.0, 10.0 })).isPressed())
					{
						// 次に進む場合は、レベルデータにもクリア情報を反映
						// これにより、Wanted シーンの初期化で自動的に次のレベルへ進む
						m_currentLevel().isCleared = m_score.isCorrect;

						// スコアを格納する
						m_currentRecords()[getData().levelIndex] = m_score;

						// 次のレベル初期化へ
						changeScene(SceneState::Wanted);
					}

					// タイトルへ戻るボタン
					if (m_gui.toResult.set(32, U"結果 / タイトルへ")
									  .setPosition(Arg::bottomLeft = Vec2{ 10.0, Scene::Height() - 10.0 })
									  .isPressed())
					{
						if (canContinue)
						{
							// 次へ行けるのにやめようとしてる人には、ダイアログを出す
							m_gui.quitAlert.open();
						}
						else
						{
							// 次のレベルが存在しないためにタイトルへ戻らなければならない場合もあるので、
							// レベルデータにもクリア情報を反映
							// （はなから間違えていれば false になるだけ）
							m_currentLevel().isCleared = m_score.isCorrect;

							// スコアを格納する
							m_currentRecords()[getData().levelIndex] = m_score;

							// 結果シーンへ
							changeScene(SceneState::Result, 1s);
						}
					}

					// ダイアログ タイトルへ戻るボタンを押して、ダイアログが開かれた場合にボタン判定が始まる
					if (m_gui.quitAlert.isPressedOK())
					{
						// スコアを格納する
						m_currentRecords()[getData().levelIndex] = m_score;

						// 結果シーンへ
						changeScene(SceneState::Result, 1s);
					}

					// ダイアログのキャンセルボタン押下監視
					m_gui.quitAlert.isPressedCancel();
				}

				break;
			}
			default: break;
		}

# if _DEBUG
		// # デバッグ機能
		if (KeyControl.pressed() and KeyShift.pressed())
		{
			// Ctrl + Shift + S で正解スキップ
			if (KeyS.pressed())
			{
				m_currentLevel().isCleared = true;
				m_currentRecords()[getData().levelIndex] = Score::LevelRecord{ getData().levelIndex + 1, true, true, 0.3, getData().levelIndex };
			
				getData().timer.reset();

				if (getData().levelIndex + 1 >= getData().levels.size())
				{
					// レベルがもう存在しない場合は、結果シーンへ
					changeScene(SceneState::Result);
				}
				else
				{
					changeScene(SceneState::Wanted);
				}
			}

			// Ctrl + Shift + R で全正解スキップ
			if (KeyR.pressed())
			{
				// 全てのレベルをクリアしたことにして結果シーンへ
				// 一気に移るので、クリアフラグを上げる必要もない
				m_currentRecords().each_index([this](size_t i, Score::LevelRecord &score) { score = Score::LevelRecord{ i + 1, true, true, 0.5, i }; });
				getData().timer.reset();
				changeScene(SceneState::Result);
			}

			// Ctrl + Shift + Q でタイマー一時停止/再開
			if (KeyQ.pressed())
			{
				if (getData().timer.isRunning())
				{
					getData().timer.pause();
				}
				else
				{
					getData().timer.resume();
				}
			}
		}
# endif
	}

	void Level::draw() const
	{
		m_bg.texture.fitted(Scene::Size()).draw();

		// # 共通処理（背面レイヤー）
		{

			for (const auto &cat : getData().spawns)
			{
				if (not cat)
				{
					continue;
				}

				// ここでも背景色に対応した影を描画
				cat->drawShadow(m_bg.shadowColor).draw();
			}
		}

		// # ステート依存処理（こっちのほうが前面）
		switch (m_state)
		{
			// ## 開始前: カウントダウン処理など
			case State::Before:	
			{
				// 3s 以下からカウントし始めたいので、残り時間がそれ以上あるときは処理しない
				if (getData().timer.s() > 3.0)
				{
					return;
				}
				// 以下、タイマーが 3s 以下であることが保証される

				// 秒数表示
				String text = U"";

				// 線形補間のパラメータ
				// sF() は小数点以下も含めた秒数、s() は切り捨ての整数秒数であることを利用して
				// 1.0 -> 0.0 に向かう値を得る
				double t = Clamp(getData().timer.sF() - getData().timer.s(), 0.0, 1.0);

				double textSize = 200.0;

				// GO! のほう
				if (getData().timer.s() == 0)
				{
					text = U"GO!";
				}
				// 3、2、1 のほう
				else
				{
					text = U"{}"_fmt(getData().timer.s());

					// テキストサイズをイージングで小さいほうに変化させる
					textSize = std::lerp(40.0, 210.0, EaseOutQuart(t));
				}

				FontAsset(Util::FontName::KoharuiroSunray)(text)
					// 枠線・影設定 -> テキストサイズ（「GO!」の時以外経過時間で縮小） -> 経過時間で透明化 -> 画面中央ぞろえ描画
					.drawAt(TextStyle::OutlineShadow(0.3, Util::Palette::Brown, Vec2{ 1.2, 1.2 }, ColorF{ 0.0, 0.65 }), textSize, Scene::Center(), ColorF{ 1.0, EaseOutExpo(t) });

				break;
			}
			// ## プレイ中: タイマー表示など
			case State::Playing:
			{
				if (getData().timer.s() > m_currentLevel().timeLimit.count())
				{
					return;
				}
				// これ以下はタイマー残り時間が 30s 以下であることを保証する

				// 中心 (60, 60) としてストップウォッチのテクスチャを最大 60px で描画
				RectF swRegion = m_gui.timer.resized(60).drawAt(Point{ 60, 60 });

				// 針の角度を計算する（時間進捗の割合 -> ラジアン）
				double angle = 2 * Math::Pi * getData().timer.sF() / m_currentLevel().timeLimit.count();

				// ### 針を描画
				// ストップウォッチの中心からちょっとずらした位置
				// 角度は反時計回りだったので、更に反転させておいた
				Line{ Vec2{ swRegion.centerX(), swRegion.centerY() + 4.0 }, Arg::angle = -angle, 16.0 }
					.draw(LineStyle::RoundCap, 4.0, Palette::Salmon);

				// ### 残り時間の描画
				FontAsset(Util::FontName::YuseiMagic)(U"のこり")
					.draw(TextStyle::Shadow(Vec2{ 1.2, 1.2 }, ColorF{ 0.2 }), 20, swRegion.tr().x + 10, swRegion.tr().y - 10, ColorF{ 1.0, Periodic::Square0_1(1s) });

				// 実際の残り時間の描画領域を取っておいて、その右にちっちゃく「秒」を描く
				RectF tRegion = FontAsset(Util::FontName::YuseiMagic)(U"{}"_fmt(getData().timer.s()))
										.drawBase(TextStyle::OutlineShadow(0.3, Util::Palette::Brown, Vec2{ 1.2, 1.2 }, ColorF{ 0.2 }), 36, Vec2{ swRegion.br().x + 10, swRegion.br().y - 5 });
				FontAsset(Util::FontName::YuseiMagic)(U"秒").drawBase(TextStyle::Shadow(Vec2{ 1.2, 1.2 }, ColorF{ 0.2 }), 24, Vec2{ tRegion.br().x + 10, swRegion.br().y - 5 });

				break;
			}
			// ## 終了: 終了表示は 3s 間 (update 参照)
			case State::Finish:
			{
				// 線形補間のパラメータ
				double t = 1.0;

				// 1.7s までの間
				if (m_watch.now() <= 1.7)
				{
					// 時間的には 1.7s だけど数値的には 1.0 より大きいので
					// 半分にしておく
					t = Clamp(m_watch.now() / 2.0, 0.0, 1.0);
				}
				// それ以外は t = 1.0 として処理

				// 「Finish!!」という表示データを一旦保存
				const DrawableText &view = FontAsset(U"KoharuiroSunray")(U"Finish!!");

				// 実際に表示される領域を取得
				const RectF &region = view.region();

				// 線形補完の始点（画面外に出ている状態）
				const Vec2 begin{ -region.w, Scene::Center().y };

				// 経過に合わせて移動させる
				// イージング関数で跳ねて戻ってくるような移動効果
				view.drawAt(TextStyle::OutlineShadow(0.3, Util::Palette::Brown, Vec2{ 1.2, 1.2 }, ColorF{ 0.0, 0.65 }), 150.0, begin.lerp(Scene::CenterF(), EaseOutElastic(t)));

				break;
			}
			case State::After:
			{
				// 背景 ちょっと暗くする
				{
					Rect{ Scene::Size() }.draw(HSV{ Util::Palette::DarkGreenAlt }.withV(0.1).withA(0.75));
				}
				// 画面左側 捕まえた猫を表示
				{
					if (m_score.isCaught)
					{
						// つかまえた猫のテクスチャを取得する
						auto &&catTexture = m_caught->get()->getTexture().scaled(m_CatTextureScale);

						// 配置する
						const auto &catRegion = catTexture.drawAt(Scene::CenterF() - SizeF(catTexture.size.x, 0));

						// 猫のテクスチャの配置を基準に、その下のほうに
						FontAsset(Util::FontName::YuseiMagic)(U"キミが捕まえた猫").drawAt(28, catRegion.centerX(), catRegion.bottomY());
					}
				}
				// 画面右側 ターゲットを表示
				{
					// つかまえた猫のテクスチャを取得する
					auto &&catTexture = TextureAsset(Cat(m_target->id)).scaled(m_CatTextureScale);

					// 配置する
					const auto &catRegion = catTexture.drawAt(Scene::CenterF() + SizeF(catTexture.size.x, 0));

					// 猫のテクスチャの配置を基準に、その下のほうに
					FontAsset(Util::FontName::YuseiMagic)(U"ターゲット").drawAt(28, catRegion.centerX(), catRegion.bottomY());
				}
				// 画面中央上部 結果表示
				{
					// 表示部分の基準座標
					const Vec2 origin{ Scene::CenterF().x, 120 };

					// 捕まえられた場合
					if (m_score.isCaught)
					{
						// 合っていた場合
						if (m_score.isCorrect)
						{
							// 明滅させたいので明度が扱える HSV
							HSV circleColor{ Color{ 249, 32, 52 } };
							HSV fontColor{ Palette::White };

							// 正解の SE が鳴っている間 明滅させる
							if (AudioAsset(Util::AudioName::SE::Correct).isPlaying())
							{
								double v = Periodic::Jump0_1(0.2s);
								circleColor.setV(v);
								fontColor.setV(v);
							}

							// 正解の円を描く
							Circle{ origin, 75 }.drawFrame(15, circleColor);

							// こはるいろサンレイのベースラインが少しずれているので位置を修正
							FontAsset(Util::FontName::KoharuiroSunray)(U"正解!!").drawAt(110, origin.withY(origin.y + 5), fontColor);
						}
						// 間違っていた場合
						else
						{
							// エイリアス
							const auto &se = AudioAsset(Util::AudioName::SE::Incorrect);

							// SE が鳴り始め = 0.0 -> 鳴り終わり = 1.0 になるような値をつくる
							double t = se.isPlaying() ? (se.posSec() / se.lengthSec()) : 1.0;

							// t を使って、不正解の音に合わせて跳ねるような動きをさせるための位置を計算
							Vec2 pos = origin.withY(20).lerp(origin, EaseOutBounce(t));

							// テキストもバツ印もちょっと跳ねる
							Shape2D::Cross(75, 25, pos).draw(Color{ 32, 70, 206, static_cast<uint8>(255 * t) });
							FontAsset(Util::FontName::KoharuiroSunray)(U"不正解...").drawAt(110, pos.withY(pos.y + 5), ColorF{ 1.0, t });
						}
					}
					else
					{
						// TODO: いい感じのアニメーションを思いつきたい
						FontAsset(Util::FontName::KoharuiroSunray)(U"時間切れ!!").drawAt(110, Scene::CenterF().x, 120);
					}

					m_gui.toResult.draw();

					// このレベルをクリアできていて、次のレベルが存在する場合のみ表示される update 参照
					m_gui.toNextLevel.draw();

					m_gui.quitAlert.draw();
				}

				break;
			}
			default: break;
		}

		BrightenCursor();
	}

	Level::~Level()
	{
		AudioAsset(getData().bgmName).stop();

		// このレベルで使用したテクスチャアセットを一旦全て解放する
		TextureAsset::ReleaseAll();
	}
}
