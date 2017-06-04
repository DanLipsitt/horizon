#include "core_padstack.hpp"
#include <algorithm>
#include <fstream>

namespace horizon {
	CorePadstack::CorePadstack(const std::string &filename, Pool &pool):
		padstack(Padstack::new_from_file(filename)),
		padstack_work(padstack),
		m_filename(filename)
	{
		rebuild();
		m_pool = &pool;
	}

	bool CorePadstack::has_object_type(ObjectType ty) {
		switch(ty) {
			case ObjectType::HOLE:
			case ObjectType::POLYGON:
			case ObjectType::POLYGON_VERTEX:
			case ObjectType::POLYGON_EDGE:
			case ObjectType::POLYGON_ARC_CENTER:
			case ObjectType::SHAPE:
				return true;
			break;
			default:
				;
		}

		return false;
	}

	int64_t CorePadstack::get_property_int(const UUID &uu, ObjectType type, ObjectProperty::ID property, bool *handled) {
		bool h = false;
		int64_t r= Core::get_property_int(uu, type, property, &h);
		if(h)
			return r;
		switch(type) {
			case ObjectType::SHAPE :
				switch(property) {
					case ObjectProperty::ID::LAYER:
						return padstack.shapes.at(uu).layer;
					default :
						return 0;
				}
			break;

			default :
				return 0;
		}
	}
	void CorePadstack::set_property_int(const UUID &uu, ObjectType type, ObjectProperty::ID property, int64_t value, bool *handled) {
		if(tool_is_active())
			return;
		bool h = false;
		Core::set_property_int(uu, type, property, value, &h);
		if(h)
			return;
		switch(type) {
			case ObjectType::SHAPE :
				switch(property) {
					case ObjectProperty::ID::LAYER:
						if(value == padstack.shapes.at(uu).layer)
							return;
						padstack.shapes.at(uu).layer = value;
					break;
					default :
						;
				}
			break;

			default :
				;
		}
		rebuild();
		set_needs_save(true);
	}

	LayerProvider *CorePadstack::get_layer_provider() {
		return &padstack;
	}

	std::map<UUID, Polygon> *CorePadstack::get_polygon_map(bool work) {
		auto &p = work?padstack_work:padstack;
		return &p.polygons;
	}
	std::map<UUID, Hole> *CorePadstack::get_hole_map(bool work) {
		auto &p = work?padstack_work:padstack;
		return &p.holes;
	}

	void CorePadstack::rebuild(bool from_undo) {
		padstack_work = padstack;
		Core::rebuild(from_undo);
	}

	void CorePadstack::history_push() {
		history.push_back(std::make_unique<CorePadstack::HistoryItem>(padstack));
	}

	void CorePadstack::history_load(unsigned int i) {
		auto x = dynamic_cast<CorePadstack::HistoryItem*>(history.at(history_current).get());
		padstack = x->padstack;
		rebuild(true);
	}

	const Padstack *CorePadstack::get_canvas_data() {
		return &padstack_work;
	}

	Padstack *CorePadstack::get_padstack(bool work) {
		return work?&padstack_work:&padstack;
	}

	std::pair<Coordi,Coordi> CorePadstack::get_bbox() {
		auto bb = padstack_work.get_bbox();
		int64_t pad = 1_mm;
		bb.first.x -= pad;
		bb.first.y -= pad;

		bb.second.x += pad;
		bb.second.y += pad;
		return bb;
	}

	void CorePadstack::commit() {
		padstack = padstack_work;
		set_needs_save(true);
	}

	void CorePadstack::revert() {
		padstack_work = padstack;
		reverted = true;
	}

	void CorePadstack::save() {
		s_signal_save.emit();

		std::ofstream ofs(m_filename);
		if(!ofs.is_open()) {
			std::cout << "can't save symbol" <<std::endl;
			return;
		}
		json j = padstack.serialize();
		ofs << std::setw(4) << j;
		ofs.close();

		set_needs_save(false);
	}
}
