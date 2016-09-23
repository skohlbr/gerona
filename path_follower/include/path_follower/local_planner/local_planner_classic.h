#ifndef LOCAL_PLANNER_CLASSIC_H
#define LOCAL_PLANNER_CLASSIC_H

/// PROJECT
#include <path_follower/local_planner/local_planner_implemented.h>

class LocalPlannerClassic : public LocalPlannerImplemented
{
public:
    LocalPlannerClassic(PathFollower& controller,
                            tf::Transformer &transformer,
                            const ros::Duration& update_interval);

    virtual void setGlobalPath(Path::Ptr path) override;

    virtual void setVelocity(geometry_msgs::Twist::_linear_type vector) override;
    virtual void setVelocity(double velocity) override;
    virtual void setParams(int nnodes, int ic, double dis2p, double dis2o, double s_angle, int ia, double lmf, int max_level) override;

protected:
    void getSuccessors(LNode*& current, std::size_t& nsize, std::vector<LNode*>& successors,
                       std::vector<LNode>& nodes, const std::vector<Constraint::Ptr>& constraints,
                       const std::vector<bool>& fconstraints, std::vector<LNode>& twins = EMPTYTWINS,
                       bool repeat = false);

    double Heuristic(const LNode& current, const double& dis2last);

    double Cost(const LNode& current, const std::vector<Scorer::Ptr>& scorer,
                const std::vector<double>& wscorer, double& score);

    double Score(const LNode& current, const std::vector<Scorer::Ptr>& scorer,
                 const std::vector<double>& wscorer);

    bool createAlternative(LNode*& s_p, LNode& alt, const std::vector<Constraint::Ptr>& constraints,
                           const std::vector<bool>& fconstraints, bool allow_lines = false);

private:
    void setDistances(LNode& current);

    void retrieveContinuity(LNode& wpose);

    void setD2P(LNode& wpose);

    bool processPath(LNode* obj,SubPath& local_wps);

    bool isInGraph(const LNode& current, std::vector<LNode>& nodes, std::size_t& asize, int& position);

    bool areConstraintsSAT(const LNode& current, const std::vector<Constraint::Ptr>& constraints,
                           const std::vector<bool>& fconstraints);

    void initConstraints(const std::vector<Constraint::Ptr>& constraints,
                         const std::vector<bool>& fconstraints);

    void setNormalizer(const std::vector<Constraint::Ptr>& constraints,
                         const std::vector<bool>& fconstraints);

    void initIndexes(Eigen::Vector3d& pose);

    void setLLP(std::size_t index);

    void setLLP();

    void retrievePath(LNode* obj, SubPath& local_wps, double& l);

    void smoothAndInterpolate(SubPath& local_wps);

    SubPath interpolatePath(const SubPath& path, double max_distance);

    void subdividePath(SubPath& result, Waypoint low, Waypoint up, double max_distance);

    SubPath smoothPath(const SubPath& path, double weight_data, double weight_smooth, double tolerance = 0.000001);

    std::vector<SubPath> segmentPath(const SubPath &path);

    SubPath smoothPathSegment(const SubPath& path, double weight_data, double weight_smooth, double tolerance);

    void setStep();

    virtual bool algo(Eigen::Vector3d& pose, SubPath& local_wps,
                     const std::vector<Constraint::Ptr>& constraints,
                     const std::vector<Scorer::Ptr>& scorer,
                     const std::vector<bool>& fconstraints,
                     const std::vector<double>& wscorer,
                     std::size_t& nnodes) override;

    virtual void printNodeUsage(std::size_t& nnodes) const override;

    virtual void printVelocity() override;

    virtual void printLevelReached() const override;

    virtual void initLeaves(LNode& root) = 0;

    virtual void updateLeaves(std::vector<LNode*>& successors, LNode*& current) = 0;

    virtual void updateBest(double& current_p, double& best_p, LNode*& obj, LNode*& succ) = 0;

    virtual void addLeaf(LNode*& node) = 0;

    virtual void reconfigureTree(LNode*& obj, std::vector<LNode>& nodes, double& best_p,
                                 const std::vector<Constraint::Ptr>& constraints,
                                 const std::vector<Scorer::Ptr>& scorer,
                                 const std::vector<bool>& fconstraints,
                                 const std::vector<double>& wscorer) = 0;

    virtual void setInitScores(LNode& wpose, const std::vector<Scorer::Ptr>& scorer,
                               const std::vector<double>& wscorer, double& dis2last) = 0;

    virtual void initQueue(LNode& root) = 0;

    virtual bool isQueueEmpty() = 0;

    virtual LNode* queueFront() = 0;

    virtual void pop(LNode*& current) = 0;

    virtual void push2Closed(LNode*& current) = 0;

    virtual void expandCurrent(LNode*& current, std::size_t& nsize, std::vector<LNode*>& successors,
                               std::vector<LNode>& nodes, const std::vector<Constraint::Ptr>& constraints,
                               const std::vector<bool>& fconstraints) = 0;

    virtual bool processSuccessor(LNode*& succ, LNode*& current,
                                  double& current_p, double& dis2last,
                                  const std::vector<Constraint::Ptr>& constraints,
                                  const std::vector<Scorer::Ptr>& scorer,
                                  const std::vector<bool>& fconstraints,
                                  const std::vector<double>& wscorer) = 0;

private:
    static constexpr double L = 0.458;//(L of Summit XL)
    static std::vector<LNode> EMPTYTWINS;

    static std::size_t nnodes_;
    static int ic_, nsucc_, li_level;
    static double TH, length_MF;
    static std::vector<double> D_THETA, RT;

    double d2p, last_s, new_s, velocity_;
    bool fvel_, b_obst;

    std::size_t index1;
    std::size_t index2;
    int r_level,n_v;

    PathInterpolated last_local_path_;

    double step_,stepc_,neig_s;
};

#endif // LOCAL_PLANNER_CLASSIC_H
