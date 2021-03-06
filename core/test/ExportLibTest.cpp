#include "config.hpp"
#include "ExportLib.cpp"

#include <boost/test/unit_test.hpp>

// Use global variable as it is used inside lambda which is passed as function.
bool isCalled;

struct ExportLibFixture {
    ExportLibFixture()
    {
        ::configure(TEST_ASSETS_PATH, TEST_ASSETS_PATH, TEST_ELEVATION_DIRECTORY,
            [](const char* message) { BOOST_FAIL(message); });
        ::registerElementBuilder("place");
    }

    void loadQuadKeys(int levelOfDetails, int startX, int endX, int startY, int endY)
    {
        isCalled = false;
        for (int i = startX; i <= endX; ++i) {
            for (int j = startY; j <= endY; ++j) {
                ::loadQuadKey(TEST_MAPCSS_DEFAULT, i, j, levelOfDetails,
                    [](const char* name, const double* vertices, int vertexCount,
                       const int* triangles, int triCount, const int* colors, int colorCount) {
                    isCalled = true;
                    BOOST_CHECK_GT(vertexCount, 0);
                    BOOST_CHECK_GT(triCount, 0);
                    BOOST_CHECK_GT(colorCount, 0);
                },
                    [](uint64_t id, const char** tags, int size, const double* vertices,
                       int vertexCount, const char** style, int styleSize) {
                    isCalled = true;
                },
                    [](const char* message) { 
                        BOOST_FAIL(message);
                    }
                );
            }
        }
        BOOST_CHECK(isCalled);
    }

    static void callback(const char* msg) { BOOST_CHECK(msg == nullptr); }

    ~ExportLibFixture()
    {
        ::cleanup();
        std::remove((std::string(TEST_ASSETS_PATH) + "string.idx").c_str());
        std::remove((std::string(TEST_ASSETS_PATH) + "string.dat").c_str());
    }
};

BOOST_FIXTURE_TEST_SUITE(ExportLib, ExportLibFixture)

BOOST_AUTO_TEST_CASE(GivenTestData_WhenAllQuadKeysAreLoadedAtZoomOne_ThenCallbacksAreCalled)
{
    ::addToInMemoryStoreInRange(TEST_MAPCSS_DEFAULT, TEST_SHAPE_NE_110M_LAND, 1, 1, callback);
    ::addToInMemoryStoreInRange(TEST_MAPCSS_DEFAULT, TEST_SHAPE_NE_110M_RIVERS, 1, 1, callback);
    ::addToInMemoryStoreInRange(TEST_MAPCSS_DEFAULT, TEST_SHAPE_NE_110M_LAKES, 1, 1, callback);

    // This data increases execution time. Also causes some issues.
    //::addToInMemoryStoreInRange(TEST_MAPCSS_DEFAULT, TEST_SHAPE_NE_110M_ADMIN, 1, 1, callback);
    //::addToInMemoryStoreInRange(TEST_MAPCSS_DEFAULT, TEST_SHAPE_NE_110M_BORDERS, 1, 1, callback);

    ::addToInMemoryStoreInRange(TEST_MAPCSS_DEFAULT, TEST_SHAPE_NE_110M_POPULATED_PLACES, 1, 1, callback);

    loadQuadKeys(1, 0, 1, 0, 1);
}

// This case tests storing lod range.
BOOST_AUTO_TEST_CASE(GivenTestData_WhenDataIsLoadedInLodRangeAtDetailedZoom_ThenCallbacksAreCalled)
{
    ::addToInMemoryStoreInRange(TEST_MAPCSS_DEFAULT, TEST_XML_FILE, 16, 16, callback);

    loadQuadKeys(16, 35205, 35205, 21489, 21489);
}

// This case tests dynamic addtion incremental addtion/search to store.
BOOST_AUTO_TEST_CASE(GivenTestData_WhenQuadKeysAreLoadedInSequenceAtDetailedZoom_ThenCallbacksAreCalled)
{
    ::addToInMemoryStoreInQuadKey(TEST_MAPCSS_DEFAULT, TEST_XML_FILE, 35205, 21489, 16, callback);
    loadQuadKeys(16, 35205, 35205, 21489, 21489);

    ::addToInMemoryStoreInQuadKey(TEST_MAPCSS_DEFAULT, TEST_XML_FILE, 35204, 21490, 16, callback);
    loadQuadKeys(16, 35204, 35204, 21490, 21490);
}

BOOST_AUTO_TEST_CASE(GivenTestData_WhenQuadKeyIsLoaded_ThenHasDataReturnsTrue)
{
    ::addToInMemoryStoreInQuadKey(TEST_MAPCSS_DEFAULT, TEST_XML_FILE, 35205, 21489, 16, callback);

    BOOST_CHECK(::hasData(35205, 21489, 16));
}

BOOST_AUTO_TEST_CASE(GivenTestData_WhenSpecificQuadKeyIsLoaded_ThenHasDataReturnsFalseForAnother)
{
    ::addToInMemoryStoreInQuadKey(TEST_MAPCSS_DEFAULT, TEST_XML_FILE, 35205, 21489, 16, callback);

    BOOST_CHECK(!::hasData(35204, 21489, 16));
}

BOOST_AUTO_TEST_SUITE_END()
