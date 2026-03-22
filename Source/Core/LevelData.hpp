# pragma once
# include "../Action.hpp"

namespace UFOCat::Core
{
	/// @brief ゲームの1フェーズに関するデータを持つクラス
	class LevelData
	{
		/* -- 構造体 -- */
	public:
		/// @brief 品種のデータ
		struct BreedData
		{
			/// @brief ターゲットと類似している猫を何匹登場させるか
			/// @details ターゲットと似ている猫の定義は、`Phase::similarity` フィールドを参照
			uint32 similar;

			/// @brief 別にターゲットとは類似ではない猫を何匹登場させるか
			uint32 other;

			/// @brief 品種の総数
			/// @return similar + other
			uint32 total() const
			{
				return similar + other;
			}
		};

		/// @brief 出現ペースのデータ
		struct IntervalData
		{
			/// @brief 1周期で登場させる猫の数
			uint32 count;

			/// @brief 1周期（インターバル）の時間
			Duration period;
		};

		/// @brief UFO猫が行うアクションのデータ
		struct ActionData
		{
			/// @brief アクション名（`CatObject` のメソッド名と同じ）
			String name;

			/// @brief メソッドを実行させるときのパラメータ情報 @n
			/// CatObject のすべての動作系メソッドの引数に対応するタプル型を格納できる
			Action::Generic params;

			/// @brief このアクションが選択される確率（0.0 ～ 1.0）
			double probability = 0.0;
		};


		/* -- フィールド -- */
	public:
		/// @brief このフェーズの制限時間
		Duration timeLimit;

		/// @brief ターゲットとの類似度 一致する特徴の数で表現する@n
		/// 例えば一致する特徴のパラメータが1つ共通するなら `1`、2つ共通するなら `2` となる
		uint32 similarity;

		/// @brief このフェーズで登場する猫の品種データ
		BreedData breedData;

		/// @brief このフェーズでの猫の出現ペース
		IntervalData intervalData;

		/// @brief このフェーズで使用し得るすべてのアクション
		Array<ActionData> actionDataList;

		/// @brief このフェーズをクリアしたかどうか
		bool isCleared = false;


		/* -- メソッド -- */
	public:
		/// @brief コンストラクタ
		/// @param timeLimit 制限時間
		/// @param similarity 類似度
		/// @param breedData 品種データ
		/// @param intervalData 出現ペース
		/// @param actionDataList 使用するすべてのアクション
		LevelData(const Duration& timeLimit, uint32 similarity, BreedData& breedData, IntervalData& intervalData, Array<ActionData>& actionDataList)
			: timeLimit{ timeLimit }
			, similarity{ similarity }
			, breedData{ breedData }
			, intervalData{ intervalData }
			, actionDataList{ actionDataList }
		{}

		/// @brief 文字列が Duration 型に変換可能かどうかを返す
		/// @param str 対象文字列
		/// @return 変換可能なら `true`
		static bool IsDuration(const String& str);

		/// @brief 文字列が Rect 型に変換可能かどうかを返す
		/// @param str 対象文字列
		/// @return 変換可能なら `true`
		static bool IsRect(const String& str);

		/// @brief 文字列が Siv3D 規定のイージング関数として見なせるかどうかを返す
		/// @param str 対象文字列
		/// @return 変換可能なら `true`
		static bool IsEasing(const String& str);

		/// @brief 文字列を Duration 型に変換する
		/// @param str 対象文字列 末尾に 's' がつく
		/// @return 変換した Duration 型オブジェクト、変換できなければ例外が投げられる
		static Duration ParseDuration(const String& str);

		/// @brief 文字列を Rect 型に変換する
		/// @param str 対象文字列 '(x, y, w, h)' の形式
		/// @return 変換した Rect 型オブジェクト、変換できなければ例外が投げられる
		static Rect ParseRect(const String& str);

		/// @brief 文字列を Siv3D 規定のイージング関数へ変換する
		/// @param str 対象文字列 'e_' で始まる必要がある
		/// @return 変換したイージング関数のオブジェクト、変換できなければ例外が投げられる
		static Action::EasingFunction ParseEasing(const String &str);

		/// @brief アクションのシグネチャをあらわしたタプル型 TSignatureTuple を指定して、アクションを実行するための引数データが格納された JSON 配列を検証する @n
		/// 検証に成功すると 指定した TSignatureTuple と同じ型でデータが格納されたタプルを返す @n
		/// このタプルを展開して CatObject のアクションメソッドに渡すことで、** JSON データからのアクションの指定を実現する **
		/// @tparam TSignatureTuple 変換したい引数の構成となるタプル型 名前空間 `cact` に定義されている各アクション（`bound` 以外）のシグネチャを指定する（`bound` は引数なしのため別途処理する）
		/// @param paramData 解析対象の JSON 配列で、TSignatureTuple の要素数と一致していなければならない @n
		/// 各要素は: 数値 (uint32 として扱う)、文字列 (Duration, Rect, または Action::EasingFunction を表す形式)、または長さ4の配列 (std::array<double,4>) であることが期待され、合わない場合は例外を投げる
		/// @return 各要素が TSignatureTuple に合う形で構成した std::tuple @n
		/// パースが成功して返ってくるこのタプルは Action::Generic に必ず当てはまる
		/// 配列の長さや各要素の型が期待と一致しない場合は例外を投げる
		template <Action::ValidSignature TSignatureTuple>
		static auto ParseParameters(const JSON &paramData)
		{
			// JSON 配列の長さと 希望のタプル型の要素数が一致していなかったらエラー
			if (paramData.size() != std::tuple_size_v<TSignatureTuple>)
			{
				throw Error(U"指定されたJSON形式のシグネチャデータが TSignatureTuple ({}) の要素数と一致していません\n"_fmt(Unicode::Widen(std::string(typeid(TSignatureTuple).name())))
							+ U"`paramData.size()`: {}\n"_fmt(paramData.size())
							+ U"`{}` length: {}"_fmt(Unicode::Widen(std::string(typeid(TSignatureTuple).name())), std::tuple_size_v<TSignatureTuple>));
			}

			// 一時的に各要素を取りうる型の variant で格納する配列（長さは TSignatureTuple 分）
			std::array<Action::VariantParam, std::tuple_size_v<TSignatureTuple>> temp;

			// 各要素を検証しながら対応する位置の temp に格納していく
			for (size_t i = 0; i < std::tuple_size_v<TSignatureTuple>; i++)
			{
				// 数値だったら uint32 として扱う
				if (paramData[i].isNumber())
				{
					temp[i] = paramData[i].get<uint32>();
				}
				// 文字列だったら Duration, Rect, または Action::EasingFunction を表す形式のいずれかであることを期待する
				else if (paramData[i].isString())
				{
					if (const String& parsed = paramData[i].get<String>();
						IsDuration(parsed))
					{
						temp[i] = ParseDuration(parsed);
					}
					else if (IsRect(parsed))
					{
						temp[i] = ParseRect(parsed);
					}
					else if (IsEasing(parsed))
					{
						temp[i] = ParseEasing(parsed);
					}
					else
					{
						throw Error(U"パラメータの変換に失敗しました。(Type: {}, ParamIndex: {})"_fmt(Unicode::FromUTF8(typeid(TSignatureTuple).name()), i));
					}
				}
				// 配列だった場合特殊ということにしておく
				// `appearFromEdge` の `overflow` でしか配列を引数にとらない
				// そのため、サイズ4の配列を決め打ちで渡して問題はないはず（`overflow` の仕様は `CatObject.hpp` を参照）
				else if (paramData[i].isArray())
				{
					std::array<double, 4> overflow{};

					if (paramData[i].size() == 4)
					{
						for (size_t j = 0; j < 4; j++)
						{
							overflow[j] = paramData[i][j].get<double>();
						}
						temp[i] = overflow;
					}
					else
					{
						throw Error(U"配列のデータが不正です。 `appearFromEdge` のパラメータに指定できる配列は [double, double, double, double] の形式です。");
					}
				}
				else
				{
					throw Error(U"不正なパラメータです。 (JSON内 index: {})"_fmt(i));
				}
			}

			// 各アクションの引数として使用可能な型が格納された配列を、指定したタプルと同じ構成に変換して返すテンプレートラムダ
			// 第1引数: 変換したい引数の構成となるタプル型を type_identity で包んで渡す
			// 第2引数: 各アクションの引数として使用可能な型が格納された配列
			// 戻り値: 指定したタプルと同じ構成に変換されたタプル
			const auto toSignatureTuple = []<typename ...Ts>
				(std::type_identity<std::tuple<Ts...>>,
				 const std::array<Action::VariantParam, sizeof...(Ts)> &variantArray)
				-> std::tuple<Ts...>
			{
				return std::apply([](auto &&...args)
				{
					// 各 variant から タプルの要素型に対応する型を取り出す
					// これを fold expression で展開して、タプルを構成する
					return std::make_tuple(std::get<Ts>(args)...);
				}, variantArray);
			};
			// std::type_identity がポイント。型データを変数のように渡すことができてテンプレートラムダで重宝する上に
			// パラメータパックでタプルの要素型をテンプレート化しやすく、テクニカル
			// そしてまさかの「sizeof...」というひとまとまりの演算子でした
			// https://cpprefjp.github.io/lang/cpp11/variadic_templates.html

			// これで戻り値が TSignatureTuple と同じ構成のタプルになる
			return toSignatureTuple(std::type_identity<TSignatureTuple>{}, temp);
		}
		// 戻り値が auto のせいでテンプレート外で定義できないので、この関数だけヘッダーに定義を書いています
	};
}
