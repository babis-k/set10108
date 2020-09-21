#include <thread>
#include <array>
#include <iostream>
#include <functional>
#include <vector>
#include <random>
#include <chrono>
#include <mutex>

using namespace std;
using namespace std::chrono;

/*
    Tasks: 
        
        Initialize a thread pool as a vector of running threads. Make sure the application exits gracefully if we input "stop" and exit the loop
            The thread pool contains two types of tasks. It's up to you to decide how many threads to devote to each type of task, and how big the thread pool is.
        
        Based on the contents of the string, call a different task:
            if the word is long (> 4 letters), run a task called variable_processor that sleeps for text.length msec
            if the word is short (<= 4 letters), run a task called fixed_processor that sleeps for 2 msec

        Try it first with input given from the user:
            when user inputs string, print the thread id and task type  (e.g. variable_processor or fixed_processor)
        
        Try it using the lorem ipsum text tokens, and experiment and find what works best for your system: 
            how big should your thread pool be before you see penalties?
            what percentage of threads should you devote for each type of task?

        Add another task that always runs (in addition to the other two) and calculates the frequencies of each word in the lorem ipsum text tokens, and prints them just before it joins
*/

struct work_pile_t
{
    // return if we managed to pop a load from the pile (we can pop only if the number is greater than 0)
    std::string Pop()
    {
        std::lock_guard<std::mutex> guard(mutex);
        auto ok = !data.empty();
        auto work_item = data.back();
        if (!data.empty())
            data.pop_back();
        return work_item;
    }

    // increment the size of the pile
    void Put(const std::string& work_item)
    {
        std::lock_guard<std::mutex> guard(mutex);
        data.push_back(work_item);
    }

    // get the size of the pile
    int Num() {
        std::lock_guard<std::mutex> guard(mutex);
        return int(data.size());
    }

private:
    std::mutex mutex;
    std::vector<std::string> data;
};


const std::vector<std::string> LoremIpsumParts() {
    std::string txt = R"(Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Massa massa ultricies mi quis hendrerit dolor magna. Duis convallis convallis tellus id interdum velit laoreet id. Nulla aliquet enim tortor at auctor urna nunc id cursus. Aliquet eget sit amet tellus cras adipiscing. Enim diam vulputate ut pharetra sit. Massa id neque aliquam vestibulum morbi blandit cursus risus. Placerat in egestas erat imperdiet sed. Arcu felis bibendum ut tristique et egestas. Nec nam aliquam sem et tortor consequat. Purus ut faucibus pulvinar elementum integer. Pulvinar neque laoreet suspendisse interdum consectetur libero id faucibus. Tincidunt eget nullam non nisi est sit amet facilisis magna. Tincidunt id aliquet risus feugiat in ante metus dictum at. Sagittis vitae et leo duis. Eget mauris pharetra et ultrices neque ornare aenean euismod. Consequat nisl vel pretium lectus quam id leo. Eget nunc scelerisque viverra mauris in aliquam sem.
At varius vel pharetra vel. Faucibus a pellentesque sit amet. Aliquet porttitor lacus luctus accumsan tortor posuere. Enim nec dui nunc mattis enim ut tellus. Amet porttitor eget dolor morbi. Tempus quam pellentesque nec nam aliquam sem et tortor consequat. Egestas integer eget aliquet nibh praesent. Turpis massa tincidunt dui ut ornare lectus sit. Commodo quis imperdiet massa tincidunt nunc pulvinar sapien et ligula. Orci nulla pellentesque dignissim enim sit amet venenatis. Scelerisque in dictum non consectetur. Sed sed risus pretium quam vulputate. Imperdiet sed euismod nisi porta lorem mollis aliquam ut porttitor. Arcu odio ut sem nulla pharetra diam sit amet nisl. Nibh sit amet commodo nulla facilisi nullam vehicula ipsum. Integer enim neque volutpat ac tincidunt vitae semper quis lectus. Lorem mollis aliquam ut porttitor.
Lectus magna fringilla urna porttitor rhoncus dolor purus. Leo vel fringilla est ullamcorper eget. Id interdum velit laoreet id donec ultrices tincidunt arcu. Velit sed ullamcorper morbi tincidunt ornare massa eget egestas purus. Massa tempor nec feugiat nisl pretium. Et malesuada fames ac turpis egestas integer eget aliquet. Aliquet eget sit amet tellus cras adipiscing enim. Cursus vitae congue mauris rhoncus aenean vel. Congue quisque egestas diam in arcu cursus. Tristique magna sit amet purus gravida quis blandit turpis cursus. Quis varius quam quisque id diam. Arcu dui vivamus arcu felis bibendum ut tristique et. Vitae tortor condimentum lacinia quis vel eros donec ac odio. Dolor morbi non arcu risus. Sit amet luctus venenatis lectus magna fringilla.
Facilisis sed odio morbi quis commodo odio aenean. Sed nisi lacus sed viverra tellus in. Tellus in metus vulputate eu scelerisque felis imperdiet. Mauris pellentesque pulvinar pellentesque habitant morbi. Etiam tempor orci eu lobortis elementum nibh tellus molestie. Diam vel quam elementum pulvinar etiam non quam lacus. Turpis tincidunt id aliquet risus feugiat in. Integer feugiat scelerisque varius morbi enim nunc faucibus a. Volutpat diam ut venenatis tellus in metus vulputate. Feugiat nisl pretium fusce id velit. Egestas integer eget aliquet nibh praesent tristique. Arcu ac tortor dignissim convallis aenean. Aliquam malesuada bibendum arcu vitae elementum curabitur vitae nunc. Cursus turpis massa tincidunt dui ut. Aliquam sem fringilla ut morbi tincidunt augue interdum velit euismod. Luctus accumsan tortor posuere ac ut consequat semper viverra. Nibh sit amet commodo nulla. Velit dignissim sodales ut eu. Ut tortor pretium viverra suspendisse potenti. Etiam tempor orci eu lobortis elementum nibh.
Pellentesque elit eget gravida cum sociis natoque penatibus. Duis convallis convallis tellus id interdum velit laoreet id donec. Tristique sollicitudin nibh sit amet commodo. Risus nullam eget felis eget nunc lobortis mattis aliquam. Erat imperdiet sed euismod nisi porta lorem mollis aliquam. Vulputate eu scelerisque felis imperdiet proin. Penatibus et magnis dis parturient montes nascetur ridiculus mus. Nisl rhoncus mattis rhoncus urna neque viverra justo nec ultrices. Eget nunc lobortis mattis aliquam faucibus purus in. Fames ac turpis egestas maecenas pharetra. Morbi tincidunt augue interdum velit. Ultricies leo integer malesuada nunc vel risus. Odio tempor orci dapibus ultrices in iaculis nunc sed augue. Vulputate odio ut enim blandit volutpat maecenas. Aenean euismod elementum nisi quis eleifend quam adipiscing vitae. Maecenas ultricies mi eget mauris pharetra et ultrices. Lacus vel facilisis volutpat est. Aenean pharetra magna ac placerat vestibulum lectus mauris ultrices eros.
Felis eget velit aliquet sagittis id. At erat pellentesque adipiscing commodo elit. Ultricies tristique nulla aliquet enim tortor at auctor. Est sit amet facilisis magna etiam tempor orci eu. Elit at imperdiet dui accumsan sit amet nulla. Duis at tellus at urna condimentum. Ipsum faucibus vitae aliquet nec ullamcorper sit amet. Varius sit amet mattis vulputate. Ut venenatis tellus in metus vulputate eu scelerisque felis. At augue eget arcu dictum varius. Sit amet mattis vulputate enim nulla. Etiam dignissim diam quis enim lobortis scelerisque.
At in tellus integer feugiat scelerisque varius morbi enim nunc. Ultrices gravida dictum fusce ut. Tempor nec feugiat nisl pretium fusce. Accumsan lacus vel facilisis volutpat est. Sit amet purus gravida quis blandit turpis. Volutpat consequat mauris nunc congue nisi vitae. Faucibus nisl tincidunt eget nullam non nisi est sit. Euismod quis viverra nibh cras pulvinar mattis nunc sed. Lorem donec massa sapien faucibus. Feugiat nibh sed pulvinar proin.
Lectus sit amet est placerat in egestas erat imperdiet sed. Pharetra vel turpis nunc eget lorem dolor. Nullam vehicula ipsum a arcu. Placerat orci nulla pellentesque dignissim enim sit amet venenatis urna. Aliquet risus feugiat in ante metus dictum. Consectetur adipiscing elit pellentesque habitant. Lacinia at quis risus sed. Suspendisse interdum consectetur libero id faucibus nisl. Adipiscing enim eu turpis egestas. Justo nec ultrices dui sapien eget. Turpis nunc eget lorem dolor sed viverra ipsum nunc aliquet. Eget velit aliquet sagittis id consectetur purus ut. Nam aliquam sem et tortor consequat id. Aliquam faucibus purus in massa tempor nec feugiat nisl pretium. Vitae et leo duis ut diam quam nulla porttitor. Eget felis eget nunc lobortis mattis. Mi eget mauris pharetra et. Et tortor consequat id porta nibh. Ultricies tristique nulla aliquet enim.
Quis varius quam quisque id diam vel quam. Faucibus a pellentesque sit amet porttitor. Feugiat nisl pretium fusce id velit ut tortor pretium viverra. Fringilla est ullamcorper eget nulla. Ac placerat vestibulum lectus mauris ultrices eros in cursus turpis. Vel pharetra vel turpis nunc. Venenatis urna cursus eget nunc. Gravida quis blandit turpis cursus in hac. Tristique risus nec feugiat in fermentum posuere urna. Condimentum id venenatis a condimentum vitae sapien pellentesque habitant. Mauris a diam maecenas sed enim. Quam vulputate dignissim suspendisse in est ante. Mauris cursus mattis molestie a iaculis at erat pellentesque adipiscing. Tortor consequat id porta nibh venenatis cras sed felis eget. Etiam sit amet nisl purus in mollis. Proin sagittis nisl rhoncus mattis rhoncus. Consequat ac felis donec et odio pellentesque.
Et ligula ullamcorper malesuada proin libero nunc consequat interdum varius. Pretium lectus quam id leo in vitae turpis massa. Nisi quis eleifend quam adipiscing. Pretium aenean pharetra magna ac placerat vestibulum lectus. Ornare arcu odio ut sem nulla pharetra diam. Morbi tristique senectus et netus et malesuada fames ac. Morbi non arcu risus quis varius. Nam aliquam sem et tortor consequat id porta. Egestas fringilla phasellus faucibus scelerisque. Maecenas ultricies mi eget mauris pharetra et ultrices. Dui accumsan sit amet nulla facilisi morbi tempus iaculis. Sed risus ultricies tristique nulla aliquet enim tortor at. Risus at ultrices mi tempus. Metus dictum at tempor commodo ullamcorper a lacus.
Elementum nisi quis eleifend quam adipiscing. Eget gravida cum sociis natoque penatibus et magnis dis parturient. Blandit volutpat maecenas volutpat blandit aliquam etiam erat. Sagittis eu volutpat odio facilisis. Mattis nunc sed blandit libero volutpat sed cras. Integer malesuada nunc vel risus commodo viverra. Pretium vulputate sapien nec sagittis. Diam maecenas sed enim ut sem viverra aliquet eget. Etiam non quam lacus suspendisse faucibus interdum posuere. Amet nisl purus in mollis nunc sed. Tristique risus nec feugiat in fermentum posuere urna nec tincidunt. Quis vel eros donec ac odio.
Erat imperdiet sed euismod nisi porta lorem mollis aliquam ut. Integer quis auctor elit sed vulputate mi. Porttitor leo a diam sollicitudin tempor id eu. Ipsum dolor sit amet consectetur. Purus sit amet volutpat consequat mauris nunc congue nisi vitae. Montes nascetur ridiculus mus mauris vitae. Sodales ut eu sem integer vitae justo eget magna. Habitasse platea dictumst quisque sagittis purus sit. Convallis convallis tellus id interdum velit laoreet. Aliquam eleifend mi in nulla posuere sollicitudin aliquam ultrices.
Maecenas volutpat blandit aliquam etiam erat velit. At augue eget arcu dictum varius duis. Volutpat blandit aliquam etiam erat velit. Scelerisque felis imperdiet proin fermentum. Convallis tellus id interdum velit laoreet id. Euismod in pellentesque massa placerat. Integer vitae justo eget magna. Amet mattis vulputate enim nulla aliquet porttitor. Risus nullam eget felis eget. Mattis molestie a iaculis at erat. Faucibus scelerisque eleifend donec pretium. Vulputate sapien nec sagittis aliquam. Ipsum faucibus vitae aliquet nec. Lorem mollis aliquam ut porttitor. Pellentesque elit ullamcorper dignissim cras tincidunt lobortis feugiat vivamus at. Viverra nibh cras pulvinar mattis nunc. Mattis ullamcorper velit sed ullamcorper morbi. Mauris rhoncus aenean vel elit.
Auctor neque vitae tempus quam pellentesque nec nam aliquam sem. Venenatis urna cursus eget nunc scelerisque viverra mauris. Pellentesque massa placerat duis ultricies lacus sed. Aliquet risus feugiat in ante metus. Sollicitudin nibh sit amet commodo nulla facilisi nullam. Sed risus pretium quam vulputate. Purus gravida quis blandit turpis cursus in hac. Aliquam vestibulum morbi blandit cursus risus. Mattis pellentesque id nibh tortor id aliquet lectus proin nibh. Condimentum mattis pellentesque id nibh tortor id. Lacus sed turpis tincidunt id aliquet risus. Eget nunc lobortis mattis aliquam faucibus. Ut venenatis tellus in metus vulputate eu scelerisque felis. Sed blandit libero volutpat sed cras ornare arcu. Vehicula ipsum a arcu cursus vitae congue. Non enim praesent elementum facilisis leo vel fringilla est. Tellus integer feugiat scelerisque varius. In mollis nunc sed id semper risus in hendrerit gravida.
Semper viverra nam libero justo laoreet sit amet cursus sit. Nulla aliquet enim tortor at auctor urna nunc id. Volutpat blandit aliquam etiam erat. Vitae purus faucibus ornare suspendisse sed nisi lacus. Et ligula ullamcorper malesuada proin libero. Feugiat in ante metus dictum at tempor commodo ullamcorper. Tincidunt lobortis feugiat vivamus at augue eget arcu. Purus in mollis nunc sed id semper risus in hendrerit. Eu sem integer vitae justo. Ullamcorper dignissim cras tincidunt lobortis feugiat vivamus at augue eget. Tempus imperdiet nulla malesuada pellentesque. Arcu cursus euismod quis viverra nibh. Phasellus vestibulum lorem sed risus ultricies tristique nulla aliquet enim. Sit amet commodo nulla facilisi nullam vehicula ipsum. Viverra nibh cras pulvinar mattis nunc sed blandit libero.
Fringilla urna porttitor rhoncus dolor purus non. Tristique sollicitudin nibh sit amet commodo. Eget egestas purus viverra accumsan in nisl nisi. A lacus vestibulum sed arcu non odio euismod. Magna fringilla urna porttitor rhoncus dolor purus non enim praesent. Turpis cursus in hac habitasse platea dictumst quisque sagittis purus. Ultrices in iaculis nunc sed augue. Egestas diam in arcu cursus euismod. Diam quam nulla porttitor massa id neque aliquam vestibulum morbi. Eget velit aliquet sagittis id consectetur purus ut faucibus pulvinar. Maecenas sed enim ut sem. Ac tortor dignissim convallis aenean et. Nullam ac tortor vitae purus faucibus ornare suspendisse sed nisi. Ultrices tincidunt arcu non sodales neque sodales ut etiam sit. Sit amet luctus venenatis lectus magna fringilla urna porttitor rhoncus. Turpis massa tincidunt dui ut ornare lectus sit. Netus et malesuada fames ac turpis egestas integer eget aliquet. Aliquam sem et tortor consequat id porta nibh venenatis cras. At ultrices mi tempus imperdiet nulla.
Dui nunc mattis enim ut tellus. Dictum non consectetur a erat nam at lectus urna duis. Viverra justo nec ultrices dui sapien eget. Velit laoreet id donec ultrices tincidunt arcu non. Lorem ipsum dolor sit amet consectetur. Morbi leo urna molestie at. Auctor urna nunc id cursus metus aliquam eleifend mi. Massa tincidunt nunc pulvinar sapien et ligula ullamcorper. Tempus egestas sed sed risus pretium. Amet facilisis magna etiam tempor orci eu lobortis elementum. Ipsum suspendisse ultrices gravida dictum fusce ut placerat. Consectetur libero id faucibus nisl tincidunt.)";
    std::remove(txt.begin(), txt.end(), '\n');
    std::remove(txt.begin(), txt.end(), '.');
    std::remove(txt.begin(), txt.end(), ',');
    std::vector<std::string> output;
    size_t off_prev = 0;
    auto off = txt.find_first_of(' ', off_prev);
    while (off != std::string::npos)
    {
        int add = output.empty() ? 0 : 1;
        output.push_back(txt.substr(off_prev+add, off - off_prev-add));
        off_prev = off;
        off = txt.find_first_of(' ', off_prev+1);
    }
    output.push_back(txt.substr(off_prev, txt.size()));
    return output;
}

static const std::vector<std::string> g_LoremIpsumParts = LoremIpsumParts();

int main(int argc, char **argv)
{
    // TODO: create threads

    // Continuous loop that reads some text and pu
    work_pile_t work_pile;

    // TODO: later on, replace this while loop with the lorem ipsum processing
    while (true)
    {
        std::string text;
        std::cin >> text;
        if (text == "stop")
            break;
        work_pile.Put(text);
    }

    // TODO: exit gracefully

    return 0;
}