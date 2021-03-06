/*
 * Copyright (c) 2021 The Foundation for Research on Information Technologies in Society (IT'IS).
 * 
 * This file is part of iSEG
 * (see https://github.com/ITISFoundation/osparc-iseg).
 * 
 * This software is released under the MIT License.
 *  https://opensource.org/licenses/MIT
 */
#include <boost/test/unit_test.hpp>

#include "../HDF5IO.h"

#include <boost/filesystem.hpp>

#include <chrono>
#include <string>

namespace fs = boost::filesystem;

BOOST_AUTO_TEST_SUITE(iSeg_suite);
BOOST_AUTO_TEST_SUITE(HDF5IO_suite);

BOOST_AUTO_TEST_CASE(WriteRead)
{
	boost::system::error_code ec;
	std::string fname = (fs::temp_directory_path() / fs::path("foo.h5")).string();
	if (fs::exists(fname, ec))
	{
		fs::remove(fname, ec);
	}

	std::string dname = "MyArray";
	size_t slice_size = 4;
	std::vector<float> data1(slice_size, 1.2f);
	std::vector<float> data2(slice_size, 2.4f);
	std::vector<float*> slices;
	slices.push_back(data1.data());
	slices.push_back(data2.data());

	iseg::HDF5IO io(4);

	{
		auto fid = io.Create(fname, false);
		BOOST_REQUIRE(fid >= 0);

		auto r = io.WriteData(fid, dname, slices.data(), 2, slice_size);
		BOOST_CHECK(r == true);

		BOOST_CHECK(io.Close(fid));
	}

	{
		auto fid = io.Open(fname);
		BOOST_REQUIRE(fid >= 0);

		std::vector<double> data(2 * slice_size);
		auto r = io.ReadData(fid, dname, 0, 2 * slice_size, data.data());
		BOOST_CHECK(r == true);

		BOOST_CHECK(io.Close(fid));
	}
	{
		auto fid = io.Open(fname);
		BOOST_REQUIRE(fid >= 0);

		std::vector<float> data(slice_size);
		size_t offset = 0;
		for (size_t i = 0; i < 2; i++, offset += slice_size)
		{
			auto r = io.ReadData(fid, dname, offset, slice_size, data.data());
			BOOST_CHECK(r == true);
		}

		BOOST_CHECK(io.Close(fid));
	}

	if (fs::exists(fname, ec))
	{
		fs::remove(fname, ec);
	}
}

BOOST_AUTO_TEST_CASE(IO_Performance)
{
	std::string dname = "MyArray";
	size_t slice_size = 1500 * 2500;
	size_t num_slices = 10;
	std::vector<float> data1(slice_size);
	for (auto& v : data1)
	{
		v = static_cast<float>(rand()) / RAND_MAX;
	}

	std::vector<float*> slices1;
	slices1.reserve(num_slices);
	for (size_t i = 0; i < num_slices; ++i)
	{
		slices1.push_back(data1.data());
	}

	std::vector<float> data(slice_size);

	{
		iseg::HDF5IO io(1);
		{
			std::string fname = (fs::temp_directory_path() / fs::path("foo.h5")).string();
			auto fid = io.Create(fname, false);
			BOOST_REQUIRE(fid >= 0);

			auto before = std::chrono::high_resolution_clock::now();
			BOOST_CHECK(io.WriteData(fid, dname, slices1.data(), num_slices, slice_size));

			// read again
			size_t offset = 0;
			for (size_t i = 0; i < num_slices; i++, offset += slice_size)
			{
				BOOST_CHECK(io.ReadData(fid, dname, offset, slice_size, data.data()));
			}
			auto const after = std::chrono::high_resolution_clock::now();
			auto ms = static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(after - before).count());

			BOOST_CHECK(io.Close(fid));

			BOOST_TEST_MESSAGE("Time " << ms << "[ms]");

			BOOST_TEST_MESSAGE("Errors: " << io.DumpErrorStack());

			boost::system::error_code ec;
			if (fs::exists(fname, ec))
			{
				fs::remove(fname, ec);
			}
		}

		io.m_ChunkSize = 1500;
		{
			std::string fname = (fs::temp_directory_path() / fs::path("foo.h5")).string();
			auto fid = io.Create(fname, false);
			BOOST_REQUIRE(fid >= 0);

			auto before = std::chrono::high_resolution_clock::now();
			BOOST_CHECK(io.WriteData(fid, dname, slices1.data(), num_slices, slice_size));

			// read again
			size_t offset = 0;
			for (size_t i = 0; i < num_slices; i++, offset += slice_size)
			{
				BOOST_CHECK(io.ReadData(fid, dname, offset, slice_size, data.data()));
			}
			auto const after = std::chrono::high_resolution_clock::now();
			auto ms = static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(after - before).count());

			BOOST_CHECK(io.Close(fid));

			BOOST_TEST_MESSAGE("Time " << ms << "[ms]");

			BOOST_TEST_MESSAGE("Errors: " << io.DumpErrorStack());

			boost::system::error_code ec;
			if (fs::exists(fname, ec))
			{
				fs::remove(fname, ec);
			}
		}
	}
}

BOOST_AUTO_TEST_SUITE_END();
BOOST_AUTO_TEST_SUITE_END();
