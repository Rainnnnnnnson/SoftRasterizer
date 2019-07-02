#include "pch.h"
#include "../SoftRasterizer/Geometry.h"
TEST(GenerateSphere, Test3X2) {
	auto datas = GenerateSphere(3, 2);
	EXPECT_EQ(datas.points.size(), 8);

	for (auto& data : datas.index) {
		EXPECT_EQ(data.point, data.normal);
	}
	EXPECT_EQ(datas.index.size(), 12);
	array<unsigned, 3> i0{2, 0, 1};
	EXPECT_EQ(datas.index[0].point, i0);
	array<unsigned, 3> i1{3, 0, 2};
	EXPECT_EQ(datas.index[1].point, i1);
	array<unsigned, 3> i2{1, 0, 3};
	EXPECT_EQ(datas.index[2].point, i2);

	array<unsigned, 3> i3{1, 4, 5};
	EXPECT_EQ(datas.index[3].point, i3);
	array<unsigned, 3> i4{1, 5, 2};
	EXPECT_EQ(datas.index[4].point, i4);
	array<unsigned, 3> i5{2, 5, 6};
	EXPECT_EQ(datas.index[5].point, i5);
	array<unsigned, 3> i6{2, 6, 3};
	EXPECT_EQ(datas.index[6].point, i6);
	array<unsigned, 3> i7{3, 6, 4};
	EXPECT_EQ(datas.index[7].point, i7);
	array<unsigned, 3> i8{3, 4, 1};
	EXPECT_EQ(datas.index[8].point, i8);

	array<unsigned, 3> i9{4, 7, 5};
	EXPECT_EQ(datas.index[9].point, i9);
	array<unsigned, 3> i10{5, 7, 6};
	EXPECT_EQ(datas.index[10].point, i10);
	array<unsigned, 3> i11{6, 7, 4};
	EXPECT_EQ(datas.index[11].point, i11);
}

TEST(GenerateSphere, Test4X3) {
	auto datas = GenerateSphere(4, 3);
	EXPECT_EQ(datas.points.size(), 14);

	for (auto& data : datas.index) {
		EXPECT_EQ(data.point, data.normal);
	}

	EXPECT_EQ(datas.index.size(), 24);
	array<unsigned, 3> i0{2, 0, 1};
	EXPECT_EQ(datas.index[0].point, i0);
	array<unsigned, 3> i1{3, 0, 2};
	EXPECT_EQ(datas.index[1].point, i1);
	array<unsigned, 3> i2{4, 0, 3};
	EXPECT_EQ(datas.index[2].point, i2);
	array<unsigned, 3> i3{1, 0, 4};
	EXPECT_EQ(datas.index[3].point, i3);

	array<unsigned, 3> i4{1, 5, 6};
	EXPECT_EQ(datas.index[4].point, i4);
	array<unsigned, 3> i5{1, 6, 2};
	EXPECT_EQ(datas.index[5].point, i5);
	array<unsigned, 3> i6{2, 6, 7};
	EXPECT_EQ(datas.index[6].point, i6);
	array<unsigned, 3> i7{2, 7, 3};
	EXPECT_EQ(datas.index[7].point, i7);
	array<unsigned, 3> i8{3, 7, 8};
	EXPECT_EQ(datas.index[8].point, i8);
	array<unsigned, 3> i9{3, 8, 4};
	EXPECT_EQ(datas.index[9].point, i9);
	array<unsigned, 3> i10{4, 8, 5};
	EXPECT_EQ(datas.index[10].point, i10);
	array<unsigned, 3> i11{4, 5, 1};
	EXPECT_EQ(datas.index[11].point, i11);

	array<unsigned, 3> i12{5, 9, 10};
	EXPECT_EQ(datas.index[12].point, i12);
	array<unsigned, 3> i13{5, 10, 6};
	EXPECT_EQ(datas.index[13].point, i13);
	array<unsigned, 3> i14{6, 10, 11};
	EXPECT_EQ(datas.index[14].point, i14);
	array<unsigned, 3> i15{6, 11, 7};
	EXPECT_EQ(datas.index[15].point, i15);
	array<unsigned, 3> i16{7, 11, 12};
	EXPECT_EQ(datas.index[16].point, i16);
	array<unsigned, 3> i17{7, 12, 8};
	EXPECT_EQ(datas.index[17].point, i17);
	array<unsigned, 3> i18{8, 12, 9};
	EXPECT_EQ(datas.index[18].point, i18);
	array<unsigned, 3> i19{8, 9, 5};
	EXPECT_EQ(datas.index[19].point, i19);

	array<unsigned, 3> i20{9, 13, 10};
	EXPECT_EQ(datas.index[20].point, i20);
	array<unsigned, 3> i21{10, 13, 11};
	EXPECT_EQ(datas.index[21].point, i21);
	array<unsigned, 3> i22{11, 13, 12};
	EXPECT_EQ(datas.index[22].point, i22);
	array<unsigned, 3> i23{12, 13, 9};
	EXPECT_EQ(datas.index[23].point, i23);
}