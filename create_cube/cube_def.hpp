#pragma once
#include <utility>
#include <tuple>
#include <unordered_map>

template<int Front,int Down,int Right,int Up,int Left,int Behind>
struct MaterialDef
{
    constexpr static int Front_ = Front;
    constexpr static int Down_ = Down;
    constexpr static int Right_ = Right;
    constexpr static int Up_ = Up;
    constexpr static int Left_ = Left;
    constexpr static int Behind_ = Behind;


    static std::array<int,6> 
    to_arr(){
        std::array<int,6> arr;
        arr[0] = Front;
        arr[1] = Down;
        arr[2] = Right;
        arr[3] = Up;
        arr[4] = Left;
        arr[5] = Behind;
        return arr;
    }
};

template<int Idx,typename Mat>
constexpr int get_material_sid()
{
    if constexpr(Idx == 0)
    {
        return Mat::Front_;
    }else
    if constexpr(Idx == 1)
    {
        return Mat::Down_;
    }else
    if constexpr(Idx == 2)
    {
        return Mat::Right_;
    }else
    if constexpr(Idx == 3)
    {
        return Mat::Up_;
    }else
    if constexpr(Idx == 4)
    {
        return Mat::Left_;
    }else
    if constexpr(Idx == 5)
    {
        return Mat::Behind_;
    }
    return -1;
}

template<typename Mat,int V,int E>
constexpr int get_material_sid_inside(int v)
{
    if(v == V)
    {
        return get_material_sid<V,Mat>();
    }else
    {
        if constexpr(V < E)
        {
            return get_material_sid_inside<Mat,V + 1,E>(v);
        }else{
            return -1;
        }
    }
}

template<typename Mat,int...IS>
constexpr int get_material_sid(int v)
{
    return get_material_sid_inside<Mat,0,6>(v);
}

template <int ID,typename Mat>
struct CubeDef
{
    constexpr static int id = ID;
    using MatTy = Mat;
};

using TUP_CUBES = std::tuple<
    CubeDef<0,MaterialDef<2,2,2,2,2,2>>, // 泥土
    CubeDef<1,MaterialDef<8,8,8,8,8,8>>, // 石块
    CubeDef<2,MaterialDef<1,1,1,1,1,1>>, // 光滑的石块
    CubeDef<3,MaterialDef<7,7,7,7,7,7>>, // 红砖墙
    CubeDef<4,MaterialDef<4,4,4,4,4,4>>, // 木板
    CubeDef<5,MaterialDef<10,10,10,10,10,10>>, // 沙子
    CubeDef<6,MaterialDef<20,20,20,20,20,20>>, // 长满苔藓的石块
    CubeDef<7,MaterialDef<16,16,16,16,16,16>>, // 金矿
    CubeDef<8,MaterialDef<18,18,18,18,18,18>>, // 煤矿
    CubeDef<9,MaterialDef<17,17,17,17,17,17>>, // 铜矿
    CubeDef<10,MaterialDef<23,23,23,23,23,23>>, // 铁矿
    CubeDef<11,MaterialDef<22,22,22,22,22,22>>, // 钻石矿
    CubeDef<12,MaterialDef<14,14,14,14,14,14>>, // 铁块
    CubeDef<13,MaterialDef<13,13,13,13,13,13>>, // 钻石块
    CubeDef<14,MaterialDef<15,15,15,15,15,15>>, // 金块
    CubeDef<15,MaterialDef<29,29,29,29,29,29>>, // 玻璃
    CubeDef<16,MaterialDef<25,25,25,25,25,25>>, // 叶子1
    CubeDef<17,MaterialDef<26,26,26,26,26,26>>, // 叶子2

    CubeDef<50,MaterialDef<3,2,3,0,3,3>>, // 草地
    CubeDef<51,MaterialDef<12,30,12,30,12,12>>, // 树干
    CubeDef<52,MaterialDef<19,4,4,4,4,4>> // 书架
>;

template<int ID,typename F,typename ... CS>
constexpr auto get_material_inside(std::tuple<F,CS...> tup)
{   
    if constexpr(ID == F::id)
    {
        using T = typename F::MatTy;
        return T();
    }else{
        if constexpr(std::tuple_size<std::tuple<CS...>>::value > 0)
        {
            return get_material_inside<ID,CS...>(std::tuple<CS...>());
        }else
        {
            return MaterialDef<6,6,6,6,6,6>();
        }   
    }
}

template<int ID,typename ... CS>
constexpr auto get_material(std::tuple<CS...> tup)
{   
    return get_material_inside<ID,CS...>(tup);
}


template<typename F,typename ... CS>
void setup_to_map_inside(std::unordered_map<int,std::array<int,6>>& map,std::tuple<F,CS...> tup)
{
    using Ty = typename F::MatTy;
    map[F::id] = Ty::to_arr();
    if constexpr(sizeof...(CS) > 0)
    {
        setup_to_map_inside(map,std::tuple<CS...>());
    }
}

template<typename ... CS>
void setup_to_map(std::unordered_map<int,std::array<int,6>>& map,std::tuple<CS...> tup)
{
    setup_to_map_inside(map,tup);
}

void test()
{
    int a = CubeDef<0,MaterialDef<0,1,2,3,4,5>>::MatTy::Front_;
    int b = get_material_sid<0,MaterialDef<0,1,2,3,4,5>>();
    int c = get_material_sid<MaterialDef<0,1,2,3,4,5>>(2);
    auto material = get_material<1>(TUP_CUBES());
}