/**
 * C++ implementation of
 * http://dx.doi.org/10.1145/362919.362945 , December 1971
 * An Extension of the Munkres Algorithm for the Assignment Problem to Rectangular Matrices
 * Francois Bourgeois and Jean-Claude Lassalle, CERN, Geneva, Switzerland
 * Comments copied from the article
 */

#ifndef DPGAnalysis_Tools_Munkres_h
#define DPGAnalysis_Tools_Munkres_h

#include <vector>
#include <utility>

template<typename t_distance_type>
class Munkres
{
protected:
    typedef std::pair<int, int> zero_sequence_pair_type;
    typedef std::vector<zero_sequence_pair_type> zero_sequence_type;
public:
    typedef t_distance_type distance_type;

protected:
    void init(std::vector<distance_type> const & _dists, int _rows, int _cols);
    void process();
    void result(std::vector<std::pair<int, int> > & _result) const;

    void find_uncovered_zero(int & _row, int & _col) const;
    distance_type find_smallest_uncovered() const;
    int find_mark_in_row(int _row, std::vector<bool> const & _marks) const;
    int find_mark_in_col(int _col, std::vector<bool> const & _marks) const;

    void step_zero();
    void step_one();
    void step_two();
    void step_three();
    void step_four();
    void step_five();

public:
    Munkres();

    void process(std::vector<distance_type> const & _dists
                 , int _rows, int _cols
                 , std::vector<std::pair<int, int> > & _result);

protected:
    static distance_type const max_;
    int step_;
    int rows_, cols_, k_, idxs_; // k_ = min(rows_, cols_), idxs_ = rows*cols
    std::vector<bool> stars_, primes_;
    std::vector<bool> row_cover_, col_cover_;
    std::vector<distance_type> dists_;
    zero_sequence_type zero_sequence_;
};

#include "DPGAnalysis/Tools/interface/Munkres.icc"

#endif // DPGAnalysis_Tools_Munkres_h
