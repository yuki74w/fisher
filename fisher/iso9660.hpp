#pragma once

#include"binary.hpp"

namespace iso9660 {
	enum class descriptor_type {
		bootrecord=0,
		pvd=1,
		svd=2,
		vpd=3,
		end=255
	};

	struct datetime {
		uint8_t year;
		uint8_t month;
		uint8_t day;
		uint8_t hour;
		uint8_t minute;
		uint8_t second;
		uint8_t utc_offset;
	};

	struct fileflag {
		bool hidden;
		bool directory;
		bool associated;
		bool has_record;
		bool ext_attribute;
		bool final_directory;
	};

	struct directoryrecord {
		uint8_t length;
		uint8_t ext_length;
		uint32_t ext_LBA;
		uint32_t data_length;
		datetime time;
		uint8_t fileunitsize;
		uint8_t interleavegapsize;
		uint16_t volumeseqnum;
		uint16_t fileidsize;
	};

	struct bootrecord {
		std::array<char, 32> systemId;
		std::array<char, 32> bootId;
		std::array<::binary::byte, 1977> custom;
	};

	namespace binary {
		namespace detail {
			::binary::bytes cast(const bootrecord& b) {
				::binary::bytes ret(static_cast<::binary::byte>(descriptor_type::bootrecord));
				ret.mode = ::binary::endian::type::little;
				ret << ::binary::bytes::option_type::not_eof << "CD001";
				ret << 1;
				ret << b.systemId;
				ret << b.bootId;
				ret << b.custom;
				return ret;
			}
		}
	}

	struct pdv {
		std::array<char, 32> systemId;
		std::array<char, 32> bootId;
		uint32_t volumesapcesize;
		uint16_t volumesetsize;
		uint16_t volumeseqnum;
		uint16_t logicalblocksize;
		uint32_t pathtablesize;
		uint32_t ltableLBA;
		uint32_t ltableoptLBA;
		uint32_t mtableLBA;
		uint32_t mtableoptLBA;


	};
}