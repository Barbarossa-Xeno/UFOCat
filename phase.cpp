#include "phase.hpp"
#include <regex>

bool Phase::updateAtInterval()
{
	// 呼び出されたときにタイマーを進めて、周期時間が経過したのを返す仕組み
	m_stopwatch.forward();
	
	return m_stopwatch.isOver(intervalData.period);
}

bool Phase::IsDuration(const String &str)
{
	// 文字列末尾が "s" だったら Duration ということに
	return str.ends_with(U"s");
}

bool Phase::IsRect(const String &str)
{
	// 正規表現で Rect と見なす
	// (数字, 数字) の形
	const std::regex rectPattern{ R"(^((\d+),\s*(\d+))$)" };
	return std::regex_match(str.narrow(), rectPattern);
}

bool Phase::IsEasing(const String &str)
{
	// 文字列戦闘が "e_" で始まっていたらイージング関数の名前ということに
	return str.substr(0, 2) == U"e_";
}

Duration Phase::ParseDuration(const String &str)
{
	// せっかく定義した Isなんちゃら を使おうと思ったけど、たかが１行なので直書きで
	if (str.ends_with(U"s"))
	{
		// "s" を削除 -> double にパース -> Duration のコンストラクタに渡す
		return Duration{ ParseFloat<double>(str.removed(U"s")) };
	}
	else
	{
		throw Error(U"String to Duration casting was failed.");
	}
}

Rect Phase::ParseRect(const String &str)
{
	// s3d::String から std::string への変換
	std::string cast = str.narrow();

	// IsRect と同じ正規表現パターンを用意
	const std::regex rectPattern{ R"(^((\d+),\s*(\d+))$)" };

	// マッチ結果を格納する変数を作って、パターンを検証
	if (std::smatch match;
		std::regex_match(cast, match, rectPattern))
	{
		return Rect
		{
			// match[1]以降が、部分パターンに適合した文字列を格納してるので 1、2 番目をそれぞれ整数に変換して Rect の幅と高さにする
			static_cast<int32>(std::stoi(match[1].str())),
			static_cast<int32>(std::stoi(match[2].str()))
		};
	}
	else
	{
		throw Error(U"String to Rect casting was failed.");
	}
}

CACT::EasingFunction Phase::ParseEasing(const String &str)
{
	if (str.substr(0, 2) == U"e_")
	{
		// "e_" を削除し、小文字にして表記揺れ統一
		auto &&removed = str.substr(2).lowercase();


		if (removed == U"back")
		{
			return Easing::Back;
		}
		else if (removed == U"bounce")
		{
			return Easing::Bounce;
		}
		else if (removed == U"circ")
		{
			return Easing::Circ;
		}
		else if (removed == U"cubic")
		{
			return Easing::Cubic;
		}
		else if (removed == U"elastic")
		{
			return Easing::Elastic;
		}
		else if (removed == U"expo")
		{
			return Easing::Expo;
		}
		else if (removed == U"Linear")
		{
			return Easing::Linear;
		}
		else if (removed == U"quad")
		{
			return Easing::Quad;
		}
		else if (removed == U"Quart")
		{
			return Easing::Quart;
		}
		else if (removed == U"sine")
		{
			return Easing::Sine;
		}
		else
		{
			throw Error(U"`{}` is not registered as easing function."_fmt(removed));
		}
	}
	else
	{
		throw Error(U"Prefix of easing function `e_` is not found.");
	}
}


