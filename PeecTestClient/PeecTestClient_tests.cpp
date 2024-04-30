#include <gtest/gtest.h>

#include "PeecTestClient.hpp"
#include "PeecQeueuLockfree.hpp"
#include "PeecFilesystem.hpp"

class TestClientFixture : public testing::Test
{
protected:
	static void SetUpTestSuite()
	{
		client.Connect("127.0.0.1", 20228);
	}

	static void TearDownTestSuite()
	{
		client.Disconnect();
	}

	static TestClient client;
};

TestClient TestClientFixture::client;

TEST_F(TestClientFixture, Connection)
{
	EXPECT_EQ(client.IsConnected(), true);
}

TEST_F(TestClientFixture, RequestServerPing)
{
	EXPECT_EQ(client.RequestServerPing()->remoteMsg.GetStatus(), MessageStatus::SUCCESS);
}

TEST_F(TestClientFixture, RequestFileRegistration)
{
	EXPECT_EQ(client.RequestFileRegistration()->remoteMsg.GetStatus(), MessageStatus::SUCCESS);
}

TEST_F(TestClientFixture, RequestFileLocation)
{
	EXPECT_EQ(client.RequestFileLocation()->remoteMsg.GetStatus(), MessageStatus::SUCCESS);
}

TEST_F(TestClientFixture, RequestGetFilesList)
{
	EXPECT_EQ(client.RequestGetFilesList()->remoteMsg.GetStatus(), MessageStatus::SUCCESS);
}

TEST_F(TestClientFixture, MessageQueue)
{
	client.RequestServerPing();
	EXPECT_EQ(client.Incoming().empty(), true);
}

TEST(TestQueueLockFree, Push)
{
	Utils::QueueLF<int> qlf;
	qlf.push_back(NULL);
	EXPECT_EQ(qlf.empty(), false);
}

TEST(TestQueueLockFree, Pop)
{
	Utils::QueueLF<int> qlf;
	qlf.push_back(NULL);
	EXPECT_EQ(qlf.pop_front(), NULL);
}

TEST(TestQueueLockFree, Empty)
{
	Utils::QueueLF<int> qlf;
	qlf.push_back(NULL);
	qlf.pop_front();
	EXPECT_EQ(qlf.empty(), true);
}
   
TEST(TestQueueLockFree, Clear)
{
	Utils::QueueLF<int>	qlf;
	
	qlf.push_back(1);
	qlf.push_back(1);
	qlf.push_back(1);
	qlf.push_back(1);

	qlf.clear();
	EXPECT_EQ(qlf.empty(), true);
}

TEST(TestQueueLockFree, PushSharedPTR)
{
	Utils::QueueLF<std::shared_ptr<int>> qlf;

	qlf.push_back(std::make_shared<int>(1));
	qlf.push_back(std::make_shared<int>(1));
	qlf.push_back(std::make_shared<int>(1));
	qlf.push_back(std::make_shared<int>(1));

	EXPECT_EQ(qlf.empty(), false);
}

TEST(TestFilePath, Normalize)
{
	FileS::PathStruct path("bar///foo");
	EXPECT_EQ(path.GetPath(), "bar/foo");
}

TEST(TestFilePath, IsDirectory)
{
	FileS::PathStruct path("bar/");
	EXPECT_EQ(path.IsDirectory(), true);
}

TEST(TestFilePath, IsRootDirectory)
{
	FileS::PathStruct pathR("C:\\");
	EXPECT_EQ(pathR.IsRootDirectory(), true);

	FileS::PathStruct pathNR("bar/");
	EXPECT_EQ(pathNR.IsRootDirectory(), false);
}

TEST(TestFilePath, IsAbsDirectory)
{
	FileS::PathStruct pathA("C:\\");
	EXPECT_EQ(pathA.IsAbsolutePath(), true);

	FileS::PathStruct pathNA("bar/");
	EXPECT_EQ(pathNA.IsAbsolutePath(), false);
}

TEST(TestFilePath, UnicodePath)
{
	FileS::PathStruct pathA("C:\\");
	EXPECT_EQ(pathA.GetPathW(), L"C:\\");
}

TEST(TestFileManager, SetWorkingDir)
{
	FileS::PathStruct workdir("C:\\");

	FileS::FileManager manager;
	EXPECT_EQ(manager.SetWorkingDir(workdir), true);
}

TEST(TestFileManager, GetWorkingDir)
{
	FileS::PathStruct workdir("C:\\");

	FileS::FileManager manager;
	manager.SetWorkingDir(workdir);
	EXPECT_EQ(manager.GetWorkingDir().GetPath(), workdir.GetPath());
}

TEST(TestFileManager, DirectoryExists)
{
	FileS::PathStruct workdir("C:\\");

	FileS::FileManager manager;
	EXPECT_EQ(manager.DirectoryExists(workdir), true);
}

TEST(TestFileManager, DirectoryNotExists)
{
	FileS::PathStruct workdir("C:\\NotExists");

	FileS::FileManager manager;
	EXPECT_EQ(manager.DirectoryExists(workdir), false);
}