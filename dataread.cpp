//
// Created by Hyunsik Yoon on 2022-04-14.
//

#include "dataread.hpp"

void dataread_gt(vector<vector<obj>>& object_per_frame
        , const string& filename
        , pair<size_t, size_t> video_size
        , string isStu003){
    ifstream file(filename);

    int max_object_id = -1;
    int max_frame_no = -1;
    int min_frame_no = 999999;
    while(file.good()){
        string object;
        getline(file, object);

        stringstream ss(object);
        string substr;


        getline(ss, substr, ',');
        try{
            if(max_frame_no < stoi(substr)){
                max_frame_no = stoi(substr);
            }

            if(min_frame_no > stoi(substr)){
                min_frame_no = stoi(substr);
            }

            getline(ss, substr, ',');

            if(max_object_id < stoi(substr)){
                max_object_id = stoi(substr);
            }
        } catch (exception) {
            break;
        }
    }

    for(size_t frame_no = min_frame_no; frame_no < max_frame_no; frame_no++){
        object_per_frame.push_back(vector<obj>());

    }
    file.close();

    ifstream file2(filename);

    bool width_factor_computed = false;
    double width_factor;
    while(file2.good())
    {
        string object;
        getline(file2, object);

        stringstream ss(object);

        //  0~3: <frame > < id > < bb_left > < bb_top >
        //  4~6: < bb_width > < bb_height > < conf >
        //  7~9: < x > < y > < z >

        //row 하나 읽어오기

        int object_id = -1;
        int frame_num = -1;
        double bb_left;
        double bb_top;
        double bb_width;
        double bb_height;
        string substr;
            try {
                getline(ss, substr, ',');
//                cout << substr <<", ";
                frame_num = stoi(substr);
                getline(ss, substr, ',');
//                cout << substr <<", ";
                object_id = stoi(substr);
                getline(ss, substr, ',');
//                cout << substr <<", ";
                bb_left = stoi(substr);
                getline(ss, substr, ',');
//                cout << substr <<", ";
                bb_top = stoi(substr);
                getline(ss, substr, ',');
//                cout << substr <<", ";
                bb_width = stoi(substr);
                getline(ss, substr, ',');
//                cout << substr <<endl;
                bb_height = stoi(substr);
                getline(ss, substr, ',');
                getline(ss, substr, ',');

                if(isStu003 != "student003_gt.txt")
                {
                    if(stoi(substr) != 7 && stoi(substr) !=1){
                        continue;
                    }
                }


                if (!width_factor_computed) {
                    width_factor_computed = true;
                    width_factor = bb_width / min((double) video_size.second, (bb_top + bb_height));
                }

                obj newobj = obj(object_id, bb_left, bb_top, bb_width, bb_height, video_size, width_factor);

//                cout << newobj.id << ", " << newobj.bb_left << endl;
                object_per_frame[frame_num - min_frame_no].push_back(newobj);
            } catch (exception) {
                break;
            }
    }

    file2.close();

}

void dataread(vector<vector<obj>>& object_per_frame
            , const string& filename
            , pair<size_t, size_t> video_size)
{
    ifstream file(filename);

    int max_object_id = -1;
    int max_frame_no = -1;
    int min_frame_no = 999999;
    while(file.good())
    {
        string object;
        getline(file, object);


        stringstream ss(object);
        string substr;

        getline(ss, substr, ' ');

        try{
            if(max_frame_no < stoi(substr))
            {
                max_frame_no = stoi(substr);
            }
            if(min_frame_no > stoi(substr))
            {
                min_frame_no = stoi(substr);
            }

            getline(ss, substr, ' ');

            if(max_object_id < stoi(substr))
            {
                max_object_id = stoi(substr);
            }
        } catch (exception) {
            break;
        }
    }


    for(size_t frame_no = min_frame_no; frame_no < max_frame_no; frame_no++)
    {
        object_per_frame.push_back(vector<obj>());
//        for(size_t obj_id = 0; obj_id < max_object_id; obj_id++)
//        {
//            object_per_frame[frame_no].push_back(obj());
//        }
    }

    file.close();

    ifstream file2(filename);
    bool width_factor_computed = false;
    double width_factor;
    while(file2.good())
    {
        string object;
        getline(file2, object);

        stringstream ss(object);

        //  0~3: <frame > < id > < bb_left > < bb_top >
        //  4~6: < bb_width > < bb_height > < conf >
        //  7~9: < x > < y > < z >

        //row 하나 읽어오기

        int object_id = -1;
        int frame_num = -1;
        double bb_left;
        double bb_top;
        double bb_width;
        double bb_height;
        string substr;




//        getline(ss, substr, ' ');
//        frame_num = stoi(substr);
//        getline(ss, substr, ' ');
//        object_id = stoi(substr);
//        getline(ss, substr, ' ');
//        bb_left = stod(substr);
//        getline(ss, substr, ' ');
//        bb_top = stod(substr);
//        getline(ss, substr, ' ');
//        bb_width = stod(substr);
//        getline(ss, substr, ' ');
//        bb_height = stod(substr);
//
//        width_factor = bb_width/min((double) video_size.second, (bb_top+bb_height));
//
//        obj newobj = obj(object_id, bb_left, bb_top, bb_width, bb_height, video_size, width_factor);
//        object_per_frame[frame_num-min_frame_no].push_back(newobj);


        while(ss.good())
        {
            try
            {
                getline(ss, substr, ' ');
                frame_num = stoi(substr);
                getline(ss, substr, ' ');
                object_id = stoi(substr);
                getline(ss, substr, ' ');
                bb_left = stod(substr);
                getline(ss, substr, ' ');
                bb_top = stod(substr);
                getline(ss, substr, ' ');
                bb_width = stod(substr);
                getline(ss, substr, ' ');
                bb_height = stod(substr);
                if(!width_factor_computed) {
                    width_factor_computed = true;
                    width_factor = bb_width / min((double) video_size.second, (bb_top + bb_height));
                }

                obj newobj = obj(object_id, bb_left, bb_top, bb_width, bb_height, video_size, width_factor);
                object_per_frame[frame_num-min_frame_no].push_back(newobj);
            } catch (exception)
            {
                break;
            }
        }
    }

    file2.close();
}

void dataprint(vector<vector<obj>>& object_per_frame)
{
    int frame_no = 0;
    for(auto& frame : object_per_frame){
        cout << "frame number: " << frame_no << endl;
        for(auto& obj : frame){
            obj.printobj();
            cout << endl;
        }
        frame_no++;
    }
}