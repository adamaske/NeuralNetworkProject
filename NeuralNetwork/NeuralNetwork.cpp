#include <iostream>
#include <vector>
#include <cmath>

class NeuronLink;
class Entity;
class Action;
#pragma region Neurons
class Neuron {
public:
    Neuron() { mPower = (rand() % 10) / 10; };
    std::vector<NeuronLink*> GetInputLinks() { return mInputLinks; };
    void SetInputLinks(std::vector<NeuronLink*> links) { mInputLinks = links; };

    std::vector<NeuronLink*> GetOutputLinks() { return mOutputLinks; };
    void SetOutputLinks(std::vector<NeuronLink*> links) { mOutputLinks = links; };
    virtual float Output() { return 2; };
    //Getting mroe lniks
    void AddInputLink(NeuronLink* link) { mInputLinks.push_back(link); };
    void AddOutputLink(NeuronLink* link) { mOutputLinks.push_back(link); };

    float GetPower() { return mPower; };
protected:
    //All the linnks connected to this neuron
    std::vector<NeuronLink*> mInputLinks;
    std::vector<NeuronLink*> mOutputLinks;
    float mPower = 0.5f;
};

class InputNeuron : public Neuron {
public:
    virtual float Output() override;
    
};
class HiddenNeuron : public Neuron {
public:
    virtual float Output() override;
};
class OutputNeuron : public Neuron {
public:
    OutputNeuron(Action* action) { mAction = action; };
    virtual float Output() override;
    void SetAction(Action* action){ mAction = action; };
    Action* GetAction() { return mAction; };
private:
    Action* mAction;
};
#pragma endregion

#pragma region Actions
class Action {
public:
    virtual void DoAction(Entity* entity);
};

class MoveForward : public Action {
public:
    virtual void DoAction(Entity* entity) override {
        entity->MoveY(1);
    };
};
class MoveBackward : public Action {
public:
    virtual void DoAction(Entity* entity) override {
        entity->MoveX(-1);
    };
};

class MoveRight : public Action {
public:
    virtual void DoAction(Entity* entity) override {
        entity->MoveX(1);
    };
};

class MoveLeft : public Action {
public:
    virtual void DoAction(Entity* entity) override {
        entity->MoveX(-1);
    };
};


#pragma endregion

class NeuronLink {
public:
    NeuronLink();
    NeuronLink(Neuron* a, Neuron* b, float weight) {
        mFirst = a;
        mLast = b;
        mFirst->AddOutputLink(this);
        mLast->AddInputLink(this);
        mWeight = weight;
    };
    Neuron* GetFirst() { return mFirst; };
    Neuron* GetLast() { return mLast; };
    float GetWeight() { return mWeight; };

    void SetFrist(Neuron* neuron) { mFirst = neuron; };
    void SetLast(Neuron* neuron) { mLast = neuron; };
    void SetWeight(float amount) { mWeight = amount; };

private:
    //Should be between -4 and 4
    float mWeight = 0;
    //A link can only go to one and from one neuron
    Neuron* mFirst;
    Neuron* mLast;
};

class Genome {
public:

};

class NeuralNet {
public:
    NeuralNet();
    NeuralNet(std::vector<InputNeuron*> input, std::vector<HiddenNeuron*> hidden, std::vector<OutputNeuron*> output) {
        mInputNeurons = input;
        mHiddenNeurons = hidden;
        mOutputNeurons = output;

        CreateLinks();
    }

    void CreateLinks() {
        NeuronLink* link;
        for (size_t i = 0; i < mInputNeurons.size(); i++)
        {
            //From every input to every hidden
            for (size_t j = 0; j < mHiddenNeurons.size(); j++)
            {
                link = new NeuronLink(mInputNeurons[i], mHiddenNeurons[i], -4 + rand() % 8);

                //From every hidden neuron to every action output
                for (size_t k = 0; k < mOutputNeurons.size(); k++)
                {
                    link = new NeuronLink(mHiddenNeurons[i], mOutputNeurons[i], -4 + rand() % 8);
                }
            }
        }
    };
    void SetInput (std::vector<InputNeuron*>  neurons){ mInputNeurons = neurons; };
    void SetHidden(std::vector<HiddenNeuron*> neurons){ mHiddenNeurons = neurons; };
    void SetOutput(std::vector<OutputNeuron*> neurons){ mOutputNeurons = neurons;};

    std::vector<InputNeuron*> GetInput() { return mInputNeurons; };
    std::vector<HiddenNeuron*> GetHidden() { return mHiddenNeurons; };
    std::vector<OutputNeuron*> GetOutput() { return mOutputNeurons; };
private:
    std::vector<InputNeuron*> mInputNeurons;
    std::vector<HiddenNeuron*> mHiddenNeurons;
    std::vector<OutputNeuron*> mOutputNeurons;
};

class Entity {
public:
    Entity(NeuralNet* net, int _x, int _y) {
        mBrain = net;
        x = _x;
        y = _y;
    };

    void DoAction() {
        //Get the highest output from the brain
        std::vector<OutputNeuron*> actions = mBrain->GetOutput();
    };
    void SetNeuralNet(NeuralNet* net) { mBrain = net; };
    NeuralNet* GetNeuralNet() { return mBrain; };

    int GetX() { return x; };
    int GetY() { return y; };

    void MoveX(int amount) { x += amount; };
    void MoveY(int amount) { y += amount; };
private:
    NeuralNet* mBrain;

    int x = 0;
    int y = 0;
};
int main()
{
    //Create neurons
    //Input has to check a thing
    std::vector<InputNeuron*> input;
    //
    std::vector<HiddenNeuron*> hidden;
    //Each output neuron has an action to it
    std::vector<OutputNeuron*> output;

    output.push_back(new OutputNeuron(new MoveForward()));
    output.push_back(new OutputNeuron(new MoveBackward()));
    output.push_back(new OutputNeuron(new MoveRight()));
    output.push_back(new OutputNeuron(new MoveLeft()));
   

    //Generations and steps
    int mGenerations = 10;
    int mStagePerGeneration = 30;

    //Zone
    int mWidth = 20;
    int mHeight = 20;
    //Survive zone
    int minX = mWidth / 2;
    int maxX = mWidth;
    int minY = mHeight / 2;
    int maxY = mHeight;

    std::vector<NeuralNet*> mBrainsToKeep;
    std::vector<Entity*> mEntities;

    //For each generation
    for (size_t i = 0; i < mGenerations; i++)
    {
        //Clear entities
        mEntities.clear();
        //Create new entities 
        for (size_t i = 0; i < 10; i++)
        {
            //If there is no brains to keep, create new brains
            if (mBrainsToKeep.size() != 0) {
                //Get a random brain from the brains to keep
                mEntities.push_back(new Entity(mBrainsToKeep[rand() % mBrainsToKeep.size()-1], 
                                                                        rand() % mWidth, rand() % mHeight));
            }
            else {
                //Create new brain with random links stuff
                mEntities.push_back(new Entity(new NeuralNet(input, hidden, output), 
                                                                            rand() % mWidth, rand() % mHeight));
            }
           
        }

        //For each stage
        for (size_t j = 0; j < mStagePerGeneration; j++)
        {
            for (size_t i = 0; i < mEntities.size(); i++)
            {
                mEntities[i]->DoAction();
            }
            //Stage completed
            std::cout << "Step " << j + 1 << " completed!" << std::endl;
        }

        //Find what brains survived
        for (size_t i = 0; i < mEntities.size(); i++)
        {
            //Is in the survive box
            if (minX < mEntities[i]->GetX() < maxX && minY < mEntities[i]->GetY() < maxY) {
                //Survive
                std::cout << "Entity " << i << " survived the generation!" << std::endl;
                mBrainsToKeep.push_back(mEntities[i]->GetNeuralNet());
            }
        }
        //Generation Completed
        std::cout << "Generation " << i + 1 << " completed!" << std::endl;
    }

    //Print status
    for (size_t i = 0; i < mEntities.size(); i++)
    {
        std::cout << "Entity " << i << ": " << mEntities[i]->GetX() << ", " << mEntities[i]->GetY() << std::endl;
    }

    std::cout << "Simulation complete\n";
    return 0;
}

float InputNeuron::Output()
{
    return mPower;
}

float HiddenNeuron::Output()
{
    float sum = 0;
    for (size_t i = 0; i < mInputLinks.size(); i++)
    {
        sum += mInputLinks[i]->GetFirst()->Output() * mInputLinks[i]->GetWeight();
    }
    return tanh(sum);
}

float OutputNeuron::Output()
{
    float sum = 0;
    for (size_t i = 0; i < mInputLinks.size(); i++)
    {
        sum += mInputLinks[i]->GetFirst()->Output() * mInputLinks[i]->GetWeight();
    }
    return tanh(sum);
}
