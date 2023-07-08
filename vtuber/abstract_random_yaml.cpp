#include "abstract_random.h"
#include "abstract_random_yaml.h"
#include <yaml-cpp/yaml.h>
#include <vector>
#include <string>
#include <type_traits>
#include <utility>
#include <glog/logging.h>
#include "namedtuple/namedtuple.h"
using namespace std;

struct RandomBoolFactoryItem {
	typedef RandomBool factory_type;
	float possibility1 = 0.5;
	MAKE_NAMEDTUPLE(possibility1)
};

struct RepeatBoolFactoryItem {
	typedef RepeatBool factory_type;
	unsigned ratio1 = 1;
	unsigned ratio0 = 1;
	MAKE_NAMEDTUPLE(ratio1, ratio0)
};

template<typename T>
void FactoryArgparse(const YAML::Node& node, T& t) {
	if (node) {
		t = node.as<T>();
	}
}

template<typename FactoryItem, unsigned ...i>
auto GenericFactoryNew(
	const YAML::Node& node,
	integer_sequence<unsigned, i...>
) {
	typedef typename FactoryItem::factory_type T;
	FactoryItem args;
	((
		(LOG(INFO) << FactoryItem::get_name(i)),
		FactoryArgparse(
			node[FactoryItem::get_name(i)],
			args.template get<i>()
		)
	), ...);
	return new T(args.template get<i>()...);
}

template<typename Base, typename FactoryItem>
Base* GenericFactoryOneItem(const YAML::Node& node) {
	if (not (node and (node.IsMap() or node.IsNull()))) {
		return nullptr;
	}
	return dynamic_cast<Base*>(
		GenericFactoryNew<FactoryItem>(
			node,
			make_integer_sequence<unsigned, FactoryItem::num_members>()
		)
	);
}

template<typename Base, typename ...FactoryItems>
Base* GenericFactory(
	const YAML::Node& node,
	const vector<string>& names
) {
	unsigned i = 0;
	Base* ret = nullptr;
	((
		ret = GenericFactoryOneItem<Base, FactoryItems>(node[names[i++]]),
		ret != nullptr
	) or ...);
	return ret;
}

namespace random_factory {

BoolPattern* FromYamlNode(const YAML::Node& node) {
	LOG(INFO) << "===";
	LOG_IF(INFO, not node.IsMap()) << "Not a map";
	return node.IsMap() ?
	(GenericFactory<
		BoolPattern, // Base
		RandomBoolFactoryItem, // Derived 1
		RepeatBoolFactoryItem // Derived 2
	>(
		node,
		{
			"RandomBool",
			"RepeatBool"
		}
	)) :
	nullptr;
}

}
