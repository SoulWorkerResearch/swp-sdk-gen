// sdk-gen.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

using namespace std;

enum class method_type
{
	none,
	ctr,
	dtr
};

struct symbol final
{
	std::string class_name{};
	std::string name{};
	method_type method_type{ method_type::none };
};

class export_unfucker final
{
private:
	static auto get_symbols(std::ranges::input_range auto&& _values)
	{
		using D = decltype(_values);
		return _values | std::views::transform(unfuck) | std::ranges::to<std::vector>();
	}

public:
	static auto form_library(const std::filesystem::path _path)
	{		
		const auto lib = reinterpret_cast<PIMAGE_DOS_HEADER>(LoadLibraryEx(_path.c_str(), NULL, DONT_RESOLVE_DLL_REFERENCES));

		assert(NULL != lib);
		assert(lib->e_magic == IMAGE_DOS_SIGNATURE);

		const auto header = reinterpret_cast<PIMAGE_NT_HEADERS>(reinterpret_cast<BYTE*>(lib) + (reinterpret_cast<PIMAGE_DOS_HEADER>(lib))->e_lfanew);

		assert(header->Signature == IMAGE_NT_SIGNATURE);
		assert(header->OptionalHeader.NumberOfRvaAndSizes > 0);

		const auto va = header->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
		const auto exports = reinterpret_cast<PIMAGE_EXPORT_DIRECTORY>(reinterpret_cast<BYTE*>(lib) + va);

		assert(exports->AddressOfNames != 0);
		const auto names = reinterpret_cast<BYTE**>((reinterpret_cast<DWORD>(lib) + exports->AddressOfNames));

		const auto values =
			views::iota(static_cast<decltype(exports->NumberOfNames)>(0), exports->NumberOfNames) |
			views::transform([&](auto&& i) { return std::string_view{ reinterpret_cast<char*>(lib) + reinterpret_cast<int>(names[i]) }; });

		return get_symbols(values);
	}

private:
	static auto unfuck(const std::string_view _line) -> symbol
	{
		symbol value{};
		value.name = _line;
		value.class_name = _line;
		return value;

		std::ispanstream stream{ _line };

		char c{};
		if (stream.read(&c, sizeof(c)); c != '?') {
			spdlog::error("bad symbol begin");
			std::exit(EXIT_FAILURE);
		}

		if (stream.read(&c, sizeof(c)); c == '?') {
			switch (c)
			{
			case '?':
			{
				switch (stream.read(&c, sizeof(c)); c)
				{
				case '0':
					value.method_type = method_type::ctr;
					break;
				default:
					break;
				}

				break;
			}

			default:
				break;
			}
		}

		while (true) {
			while (stream.read(&c, sizeof(c))) {
				if ('@' == c || (not std::isalpha(c) && not std::isdigit(c))) break;
				value.name.push_back(c);
			}

			break;
		}

		return value;
	}
};

auto generate_files(const std::span<const symbol> _values, const std::filesystem::path _path) -> bool
{
	 filesystem::path path{ SOLUTION_DIR };
	 path /= "swp-sdk";
	 path /= "swp-sdk";
	 path /= "headers";
	 path /= "swpsdk";
	 path /= "natives";
	 path /= _path;

	 if (not filesystem::create_directories(path) || not filesystem::exists(path)) {
		 spdlog::critical("directory not exsts and can't create it");
		 return false;
	 }

	auto q = _values | std::views::chunk_by([](auto& l, auto& r) { return l.class_name == r.class_name; });
	std::ranges::for_each(q, [path](const auto& _values)
		{
			const auto a_class = std::ranges::any_of(_values, [](const auto& v) { return v.class_name != ""; });
			if (not a_class) return;

			std::ofstream file { path / std::format("{}.hpp", _values[0].class_name) };
			std::ranges::for_each(_values, [](const symbol& value)
				{
				});
		});

	return true;
}

auto main(void) -> int32_t
{
	auto path{ TEXT("C:\\Program Files (x86)\\Steam\\steamapps\\common\\Soulworker_KR\\GamePlugin.vPlugin") };

	const auto values = export_unfucker::form_library(path);
	generate_files(values, "game_plugin");

	// "??0CItem@@QAE@ABV0@@Z"

	return EXIT_SUCCESS;
}
