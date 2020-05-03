#define TILE_SIZE 16
/*void MulMatricesGpu(std::vector<int> v1, std::vector<int> v2, std::vector<int> vOut, int m, int n){

    Concurrency::extent<2> ext(m, n);

    Concurrency::array_view<int, 2> a(ext, v1);
    Concurrency::array_view<int, 2> b(ext, v2);
    Concurrency::array_view<int, 2> c(ext, vOut);

    Concurrency::index<2> id(0, 0);

    parallel_for_each(ext, [=](Concurrency::index<2> idx)restrict(amp){
        int row = idx[0];
        int col = idx[i];
        int sum = 0;
        //c[idx] = a[idx] + b[idx];
    });
}*/
/*
void MulMatrixCpu(std::vector<int> v1, std::vector<int> v2, std::vector<int> vOut, int m, int n){
    for(int row = 0; row < m; ++row){
        for(int col = 0; col < m; ++col){
            int sum = 0;
            for(int i = 0; i < n; ++i){
                sum += v1[row * n + i] * v2[i * n + col];
            }
            vOut[row * n + col] = sum;
        }
    }
}

void MulMatrixGpu(std::vector<int> v1, std::vector<int> v2, std::vector<int> vOut, int m, int n){
    Concurrency::extent<2> ext(m, n);
    Concurrency::array_view<int, 2> a(ext, v1);
    Concurrency::array_view<int, 2> b(ext, v2);
    Concurrency::array_view<int, 2> c(ext, vOut);
    c.discard_data();

    parallel_for_each(ext, [=](Concurrency::index<2> id)restrict(amp){
        int row = id[0];
        int col = id[1];

        int sum = 0;
        for(int i = 0; i < b.extent[0]; ++i){
            sum += a(row, i) * b(i, col);
        }
        c[id] = sum;  
    });
    c.synchronize();
}


void MulMatrixGpu2(std::vector<int> v1, std::vector<int> v2, std::vector<int> vOut, int m, int n){
    Concurrency::extent<2> ext(m, n);
    Concurrency::array_view<const int, 2> a(m, n, v1), b(m, n, v2);
    Concurrency::array_view<int, 2> c(m, n, vOut);
    c.discard_data();


    parallel_for_each(c.extent.tile<TILE_SIZE,TILE_SIZE>(), [=](Concurrency::tiled_index<TILE_SIZE,TILE_SIZE> id)restrict(amp){
        tile_static int loc_cache_a[TILE_SIZE][TILE_SIZE];
        tile_static int loc_cache_b[TILE_SIZE][TILE_SIZE];

        int tl_row = id.local[0];
        int tl_col = id.local[1];

        int sum = 0;

        for(int i = 0; i < c.extent[1]; i+=TILE_SIZE){
            //precache matrix data
            loc_cache_a[tl_row][tl_col] = a(id.global[0], i + tl_col);
            loc_cache_b[tl_row][tl_col] = a(tl_row + i, id.global[1]);
            //wait all threads to complete precache
            id.barrier.wait();
            //compute partial dot (for precached block)
            for(int b = 0; b < TILE_SIZE; b++){
                sum += loc_cache_a[tl_row][b] * loc_cache_b[b][tl_col];
            }

            id.barrier.wait();
        }

        c[id.global] = sum;
    });
    c.synchronize();
}*/


   /* const int m = 1024;
    const int n = 1024;

    std::vector<int> v1(m * n);
    std::vector<int> v2(m * n);
    std::vector<int> vr1(m * n);
    std::vector<int> vr2(m * n);
    std::vector<int> vr3(m * n);

    int i = 0;
    std::generate(v1.begin(), v1.end(), [&i]() mutable -> int {return i++;} );
    std::generate(v2.begin(), v2.end(), [&i]() mutable -> int {return i--;} );


    for(int i = 0; i <10; i++){
        timer.Start();
        MulMatrixGpu(v1, v2, vr1, m, n);
        timer.Stop();
        std::cout << "MulMatrixGpu time(s): " << timer.getTimeElapsed() << std::endl;

        timer.Start();
        MulMatrixGpu2(v1, v2, vr2, m, n);
        timer.Stop();
        std::cout << "MulMatrixGpu2 time(s): " << timer.getTimeElapsed() << std::endl;

        timer.Start();
        MulMatrixCpu(v1, v2, vr3, m, n);
        timer.Stop();
        std::cout << "MulMatrixCpu time(s): " << timer.getTimeElapsed() << std::endl;
    }

    std::cout << "Done" << std::endl;
    std::cin.get();
    for(int i = 0; i < m*n; i++){
        int sum = vr1[i] + vr2[i] + vr3[i];
        std::cout << sum;
    }*/

    
/*auto accelerators = Concurrency::accelerator::get_all();
    for(auto a : accelerators){
        std::wcout << a.get_description() << std::endl;
        std::wcout << "\tPath: " << a.device_path << std::endl;
        std::wcout << "\tMemory (MB): " << a.get_dedicated_memory()/ 1024 << std::endl;
        std::wcout << "\tHas display: " << a.get_has_display() << std::endl;
        std::wcout << "\tIs debug: " << a.is_debug << std::endl;
        std::wcout << "\tIs emulated: " << a.is_emulated << std::endl;
        
    }
    Concurrency::accelerator def_acc;*/
   // Concurrency::accelerator::set_default()
   //std::wcout << "Default: " << def_acc.device_path << std::endl;

