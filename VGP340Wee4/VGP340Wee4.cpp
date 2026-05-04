// VGP340Wee4.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <vector>
#include <mutex>
#include <thread>
#include <chrono>
#include <random>
#include <future>
#include <cmath>
#include <atomic>
#include <limits>

class Entity
{
public: 
    Entity(const std::string& name, float x, float y, float z)
        : mName(name)
        , mX(x)
        , mY(y)
        , mZ(z)
    {

    }
    void Update(float deltaTime)
    {
        // move horizontal 
        float moveX = (rand() % 21) - 10;
        float moveZ = (rand() % 21) - 10;
        float distance = sqrtf(moveX * moveX + moveZ * moveZ);
        if (std::abs(distance) > 0.01f)
        {
            moveX /= distance;
            moveZ /= distance;
        }
        mX += moveX * deltaTime;
        mZ += moveZ * deltaTime;
        std::cout << "Update: " << mName << "move a distance of (" << distance << ")\n";
    }
    void Render()
    {
        std::cout << "Render: " << mName << " is at position (" << mX << ", " << mY << ", " << mZ << ")\n";
    }
    const std::string& GetName() const { return mName; }
private: 
    std::string mName = "";
    float mX = 0.0f;
    float mY = 0.0f;
    float mZ = 0.0f;
    bool mHasRenderData = false;
};

//Create a class (Singleton if you want practice) EntityManager​
//    std::vector<Entity> mEntities; ​
//    AddEntity(Entity & entity); ​
//    RemoveEntity(const std::string & name); ​
//    Entity& GetEntities(); 

class EntityManager
{
public:
    void AddEntity(Entity& entity)
    {
        mEntities.push_back(entity);
    }
    void RemoveEntity(Entity& entity)
    {
        auto iter = std::find_if(mEntities.begin(), mEntities.end(), [entity](Entity& e)
            {
                return e.GetName() == entity.GetName();
            });
        if (iter != mEntities.end())
        {
            mEntities.erase(iter);
        }
    }
    std::vector<Entity>& GetEntities() { return mEntities; }
private:
    std::vector<Entity> mEntities;
};
//Create two class​
//    Simulation​
//        EntityManager* mEntityManager; ​
//        std::mutex* mMutex; ​
//        Initialize(EntityManager & em, std::mutex & mutex); //pass in variable if not using singleton, store as a EntityManager* mEntityManager​
//        Update(); //Update that calls all of the entities update with a 1/60 deltaTime // is a thread and will sleep_for() 17 milliseconds (roughly 60 fps) ​
class Simulation
{
public:
    void Initialize(EntityManager& em, std::mutex& mutex)
    {
        mEntityManager = &em;
        mGameMutex = &mutex;
        mIsRunning = true;
    }
    void Update()
    {
        std::chrono::milliseconds sleepTime(1000);
        const float deltaTime = 1.0f / 60.0f;
        while (mIsRunning)
        {
            mGameMutex->lock();
            system("cls");
            std::cout << "Running Update\n";
            std::vector<Entity>& entities = mEntityManager->GetEntities();
            for (Entity& e : entities)
            {
                e.Update(deltaTime);
            }
            std::this_thread::sleep_for(sleepTime);
            mGameMutex->unlock();
            std::this_thread::sleep_for(sleepTime);
            // if you were to make a game frame based
            // after each loop, you'll do a time check, then loop for the frames to catch up

            //auto startTime = chrono::now()
            //Update(deltaTime)
            //auto updateDuration = chrono::now() - starTime
            //sleep_for(sleepTime - updateDuration); (if updated < 60 fps)
            //if renderupdate > 60 fps:
            //while(frameDuration >= deltaTime)
            //  Update(deltaTime);
            //  frameDuration -= deltaTime
            //sleep_for(sleepTime)
        }
    }
    void Terminate()
    {
        mIsRunning = false;
    }
private:
    EntityManager* mEntityManager = nullptr;
    std::mutex* mGameMutex = nullptr;
    bool mIsRunning = false;
};
//    Render​
//        EntityManager* mEntityManager; ​
//        std::mutex* mMutex; ​
//        Initialize(EntityManager & em, std::mutex & mutex); //pass in variable if not using singleton, store as a EntityManager* mEntityManager​
//        Render(); // Renders all of the entities // is a thread and will sleep_for() 33 ms (roughly 30 fps), use system("cls") each update​
class Render
{
public:
    void Initialize(EntityManager& em, std::mutex& mutex)
    {
        mEntityManager = &em;
        mGameMutex = &mutex;
        mIsRunning = true;
    }
    void RenderEntites()
    {
        std::chrono::milliseconds sleepTime(1000);
        while (mIsRunning)
        {
            mGameMutex->lock();
            system("cls");
            std::cout << "Running Render\n";
            std::vector<Entity>& entities = mEntityManager->GetEntities();
            for (Entity& e : entities)
            {
                e.Render();
            }
            std::this_thread::sleep_for(sleepTime);
            mGameMutex->unlock();
            std::this_thread::sleep_for(sleepTime);
        }
    }

    void Terminate()
    {
        mIsRunning = false;
    }
private:
    EntityManager* mEntityManager = nullptr;
    std::mutex* mGameMutex = nullptr;
    bool mIsRunning = false;
};

//Test​
//    Create EntityManager, Simulation, and Render classes​
//    Add 20 entities to the entity manager(give names and random positions xz between –100.0f and 100.0f with a y 0.0f)​
//    Create threads for Simulation and Render(thread simulationThread(&Simulation::Run, &simulation);)​
//    Run until you press a button to exit(system("pause"))​

float RandFloat()
{
    // creates a random float between -100 to 100
    float val = (rand() % 201) - 100;
    return val;
}

void GameLoopExample()
{
    std::cout << "Game Simulation\n";

    std::mutex gameMutex;
    EntityManager entityManager;
    Simulation sim;
    Render render;

    sim.Initialize(entityManager, gameMutex);
    render.Initialize(entityManager, gameMutex);

    for (int i = 0; i < 4; ++i)
    {
        std::string name = "Player " + std::to_string(i);
        Entity newEntity(name, RandFloat(), 0.0f, RandFloat());
        entityManager.AddEntity(newEntity);
    }

    for (int i = 0; i < 16; ++i)
    {
        std::string name = "Enemy " + std::to_string(i);
        Entity newEntity(name, RandFloat(), 0.0f, RandFloat());
        entityManager.AddEntity(newEntity);
    }

    std::thread simThread(&Simulation::Update, &sim);
    std::thread renderThread(&Render::RenderEntites, &render);

    system("pause");
    sim.Terminate(); // flag the thread to end
    render.Terminate(); // flag the thread to end

    renderThread.join();
    simThread.join();

}

// thread safe flag to indicate data input is finished
//std::atomic_bool done = false; // thread safe flag to indicate data input is finished


//struct CircularBuffer
//{
//    int* buffer;
//    int capacity;
//
//    int frontIndex;
//    int rearIndex;
//    int count;
//    std::mutex mutex;
//    std::condition_variable notEmpty;
//    std::condition_variable notFull;
//
//    CircularBuffer(int cap)
//        : capacity(cap)
//        , frontIndex(0)
//        , rearIndex(0)
//        , count(0)
//        , buffer(new int[cap])
//    {
//
//    }
//
//    void Push(int num)
//    {
//        std::unique_lock<std::mutex> lk(mutex);
//        notFull.wait(lk, [this]() { return count != capacity; });
//        buffer[rearIndex] = num;
//        rearIndex = (rearIndex + 1) % capacity;
//        ++count;
//        lk.unlock();
//        //trigger sending a message it is not empty 
//        notEmpty.notify_one();
//    }
//    int Pop()
//    {
//        std::unique_lock<std::mutex> lk(mutex);
//        notEmpty.wait(lk, [this]() { return count > 0; });
//        int data = buffer[frontIndex];
//        frontIndex = (frontIndex + 1) % capacity;
//        --count;
//
//        lk.unlock();
//        notFull.notify_one();
//        return data;
//    }
//};

//std::mutex bufferMutex;
//int totalMin = INT_MAX;
//int totalMax = INT_MIN;
//void ConsumerFunction(CircularBuffer& buffer, int id)
//{
//    int localMin = INT_MAX;
//    int localMax = INT_MIN;
//    for (int i = 0; i < 950; ++i)
//    {
//        int value = buffer.Pop();
//        localMin = std::min(localMin, value);
//        localMax = std::max(localMax, value);
//    }
//    {
//        std::lock_guard<std::mutex> lk(bufferMutex);
//        std::cout << "Consumer " << id << ": local max [" << localMax << "] local min [" << localMin << "]\n";
//        totalMin = std::min(localMin, totalMin);
//        totalMax = std::max(localMax, totalMax);
//    }
//}

//void ProducerFunction(CircularBuffer& buffer, int id)
//{
//    auto sleepTime = std::chrono::milliseconds(10);
//    std::mt19937 randomGenerator((unsigned int)std::chrono::system_clock::now().time_since_epoch().count());
//    for (int i = 0; i < 1000; ++i)
//    {
//        buffer.Push(randomGenerator() % 100000);
//        std::this_thread::sleep_for(sleepTime);
//    }
//}
//
//void Exercise2()
//{
//    CircularBuffer dataBuffer(400);
//    std::thread consumer1(ConsumerFunction, std::ref(dataBuffer), 1);
//    std::thread consumer2(ConsumerFunction, std::ref(dataBuffer), 2);
//    std::thread producer1(ProducerFunction, std::ref(dataBuffer), 1);
//    std::thread producer2(ProducerFunction, std::ref(dataBuffer), 2);
//
//    producer2.join();
//    producer1.join();
//    consumer2.join();
//    consumer1.join();
//
//    std::cout << "Total Max: [" << totalMax << "] Total Min: [" << totalMin << "]\n";
//}
//
//struct Result
//{
//    int minValue = INT_MAX;
//    int maxValue = INT_MIN;
//};
//Result ConsumerFunctionAsync(CircularBuffer& buffer, int id)
//{
//    Result result;
//    for (int i = 0; i < 950; ++i)
//    {
//        int value = buffer.Pop();
//        result.minValue = std::min(result.minValue, value);
//        result.maxValue = std::max(result.maxValue, value);
//    }
//
//    return result;
//}
//

//int main()
//{
//    CircularBuffer dataBuffer(400);
//    //std::async<Result> consumer1(ConsumerFunction, std::ref(dataBuffer), 1);
//    std::future<Result> consumer1Result = std::async(std::launch::async, ConsumerFunctionAsync, std::ref(dataBuffer), 1);
//    std::future<Result> consumer2Result = std::async(std::launch::async, ConsumerFunctionAsync, std::ref(dataBuffer), 2);
//    std::thread producer1(ProducerFunction, std::ref(dataBuffer), 1);
//    std::thread producer2(ProducerFunction, std::ref(dataBuffer), 2);
//
//    Result result1 = consumer1Result.get();
//    Result result2 = consumer2Result.get();
//    producer2.join();
//    producer1.join();
//
//    std::cout << "Result 1 Max: [" << result1.maxValue << "] Min: [" << result1.minValue << "]\n";
//    std::cout << "Result 2 Max: [" << result2.maxValue << "] Min: [" << result2.minValue << "]\n";
//    totalMax = std::max(totalMax, result1.maxValue);
//    totalMax = std::max(totalMax, result2.maxValue);
//    totalMin = std::min(totalMin, result1.minValue);
//    totalMin = std::min(totalMin, result2.minValue);
//
//    std::cout << "Total Max: [" << totalMax << "] Total Min: [" << totalMin << "]\n";
//    return 0;
//}





//---------------------- Assignment 4 Closest Particles ------------------------------------

std::atomic_bool done = false;

struct Point
{
    float x = 0.0f;
    float y = 0.0f;
};

struct CircularBuffer
{
    Point* buffer;
    int capacity;

    int frontIndex;
    int rearIndex;
    int count;
    std::mutex mutex;
    std::condition_variable notEmpty;
    std::condition_variable notFull;

    CircularBuffer(int cap)
        : capacity(cap)
        , frontIndex(0)
        , rearIndex(0)
        , count(0)
        , buffer(new Point[cap])
    {

    }

    void Push(const Point& p)
    {
        std::unique_lock<std::mutex> lk(mutex);
        notFull.wait(lk, [this]() { return count != capacity; });
        buffer[rearIndex] = p;
        rearIndex = (rearIndex + 1) % capacity;
        ++count;
        lk.unlock();
        //trigger sending a message it is not empty 
        notEmpty.notify_one();
    }

    Point Pop()
    {
        std::unique_lock<std::mutex> lk(mutex);
        notEmpty.wait(lk, [this]() { return count > 0 || done.load(); });
        if (count == 0)
            return Point{};
        Point data = buffer[frontIndex];
        frontIndex = (frontIndex + 1) % capacity;
        --count;
        lk.unlock();
        notFull.notify_one();
        return data;
    }

};

void ProducerFunction(CircularBuffer& buffer, int id)
{
    auto sleepTime = std::chrono::milliseconds(30);
    std::mt19937 randomGenerator((unsigned int)std::chrono::system_clock::now().time_since_epoch().count());
    std::uniform_real_distribution<float> dist(-1000.0f, 1000.0f);

    for (int i = 0; i < 10000; ++i)
    {
        Point p;
        p.x = dist(randomGenerator);
        p.y = dist(randomGenerator);
        buffer.Push(p);
        std::this_thread::sleep_for(sleepTime);
    }

    // signal all consumers that production is finished
    done.store(true);
    buffer.notEmpty.notify_all();
}

float Distance(const Point& a, const Point& b)
{
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    return sqrtf(dx * dx + dy * dy);
}

bool FindClosestPair(const std::vector<Point>& pts, Point& outA, Point& outB, float& outDist)
{
    if (pts.size() < 2)
        return false;

    outDist = std::numeric_limits<float>::max();
    for (size_t i = 0; i < pts.size(); ++i)
    {
        for (size_t j = i + 1; j < pts.size(); ++j)
        {
            float d = Distance(pts[i], pts[j]);
            if (d < outDist)
            {
                outDist = d;
                outA = pts[i];
                outB = pts[j];
            }
        }
    }
    return true;
}

bool PointBelongsToQuarter(const Point& p, int quarterIndex)
{
    switch (quarterIndex)
    {
    case 1: return p.x >= 0.0f && p.y >= 0.0f;
    case 2: return p.x < 0.0f && p.y >= 0.0f;
    case 3: return p.x < 0.0f && p.y < 0.0f;
    case 4: return p.x >= 0.0f && p.y < 0.0f;
    default: return false;
    }
}

struct QuarterResult
{
    int   quarterIndex = 0;
    int   totalPoints = 0;
    Point closest1;
    Point closest2;
    float closestDistance = std::numeric_limits<float>::max();
};

QuarterResult ConsumerFunctionAsync(CircularBuffer& buffer, int quarterIndex)
{
    QuarterResult result;
    result.quarterIndex = quarterIndex;

    std::vector<Point> localPoints;

    while (!done.load() || buffer.count > 0)
    {
        Point p = buffer.Pop();
        if (p.x == 0.0f && p.y == 0.0f && buffer.count == 0 && done.load())
            break;
        if (PointBelongsToQuarter(p, quarterIndex))
            localPoints.push_back(p);
    }

    result.totalPoints = (int)localPoints.size();
    FindClosestPair(localPoints, result.closest1, result.closest2, result.closestDistance);

    return result;
}

int main()
{
    CircularBuffer dataBuffer(200);

    std::future<QuarterResult> consumer1Result = std::async(std::launch::async, ConsumerFunctionAsync, std::ref(dataBuffer), 1);
    std::future<QuarterResult> consumer2Result = std::async(std::launch::async, ConsumerFunctionAsync, std::ref(dataBuffer), 2);
    std::future<QuarterResult> consumer3Result = std::async(std::launch::async, ConsumerFunctionAsync, std::ref(dataBuffer), 3);
    std::future<QuarterResult> consumer4Result = std::async(std::launch::async, ConsumerFunctionAsync, std::ref(dataBuffer), 4);

    std::thread producer1(ProducerFunction, std::ref(dataBuffer), 1);

    QuarterResult result1 = consumer1Result.get();
    QuarterResult result2 = consumer2Result.get();
    QuarterResult result3 = consumer3Result.get();
    QuarterResult result4 = consumer4Result.get();

    producer1.join();

    const char* signs[4] = { "(+, +)", "(-, +)", "(-, -)", "(+, -)" };

    QuarterResult results[4] = { result1, result2, result3, result4 };
    for (int i = 0; i < 4; ++i)
    {
        QuarterResult& r = results[i];
        if (r.totalPoints < 2)
        {
            std::cout << "quarter " << r.quarterIndex << ": not enough points (total: " << r.totalPoints << ")\n\n";
        }
        else
        {
            std::cout << "quarter " << r.quarterIndex << ": closest points are ("
                << r.closest1.x << ", " << r.closest1.y
                << ") and ("
                << r.closest2.x << ", " << r.closest2.y
                << ") and their distance is "
                << r.closestDistance << ". "
                << "Total number of point in this quarter is "
                << r.totalPoints << ".\n\n";
        }
    }

    return 0;
}