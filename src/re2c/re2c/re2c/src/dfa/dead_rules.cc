#include <string.h>
#include <set>
#include <string>
#include <valarray>
#include <vector>

#include "src/conf/warn.h"
#include "src/dfa/dfa.h"
#include "src/re/rule.h"
#include "src/util/forbid_copy.h"

namespace re2c
{

struct tcmd_t;

/* note [unreachable rules]
 *
 * DFA may contain useless final states. Such states may
 * appear as a result of:
 *   - (part of) one rule being shadowed by another rule,
 *     e.g. rule [ab] partially shadows [ac] and completely
 *     shadows [a]
 *
 *   - infinite rules that greedily eat all input characters
 *     and never stop (they either fail on YYFILL or crash),
 *     e.g. [^]*
 *
 *   - rules that contain never-matching link, e.g. '[]'
 *     with option '--empty-class match-none'
 *
 * Useless final states should be eliminated so that they
 * don't interfere with further analyses and optimizations.
 * If all final states of a rule are useless, then the whole
 * rule is unreachable and should be reported.
 *
 * In order to find out if a given final state is useless,
 * we have to find out if all outgoing paths from this state
 * match longer rules (otherwise, some paths go to default
 * state and fallback to this state). We do this by finding
 * all states that have transitions to default state and back
 * propagation of "none-rule" from these states. As the back
 * propagation meets the first final state on its way, it
 * substitutes "none-rule" with the corresponding rule,
 * which is further propagated back to the start state of DFA.
 */


/* note [fallback states]
 *
 * Find states that are accepting, but may be shadowed
 * by other accepting states: when the short rule matches,
 * lexer must try to match longer rules; if this attempt is
 * unsuccessful it must fallback to the short match.
 *
 * In order to find fallback states we need to know if
 * "none-rule" is reachable from the given state, the information
 * we have after rule liveness analyses. Fallback states are
 * needed at different points in time (both before and after
 * certain transformations on DFA). Fortunately, fallback states
 * are not affected by these transformations, so we can calculate
 * them here and save for future use.
 */


// reversed DFA
struct rdfa_t
{
    struct arc_t
    {
        size_t dest;
        arc_t *next;
    };

    struct state_t
    {
        arc_t *arcs;
        size_t rule;
        bool fallthru;
    };

    size_t nstates;
    size_t nrules;
    state_t *states;
    arc_t *arcs;

    explicit rdfa_t(const dfa_t &dfa)
        : nstates(dfa.states.size())
        , nrules(dfa.rules.size())
        , states(new state_t[nstates]())
        , arcs(new arc_t[nstates * dfa.nchars])
    {
        // init states
        for (size_t i = 0; i < nstates; ++i) {
            state_t &s = states[i];
            s.arcs = NULL;
            const size_t r = dfa.states[i]->rule;
            s.rule = r == Rule::NONE ? nrules : r;
            s.fallthru = false;
        }
        // init arcs
        arc_t *a = arcs;
        for (size_t i = 0; i < nstates; ++i) {
            dfa_state_t *s = dfa.states[i];
            for (size_t c = 0; c < dfa.nchars; ++c) {
                const size_t j = s->arcs[c];
                if (j != dfa_t::NIL) {
                    a->dest = i;
                    a->next = states[j].arcs;
                    states[j].arcs = a++;
                } else {
                    states[i].fallthru = true;
                }
            }
        }
    }

    ~rdfa_t()
    {
        delete[] states;
        delete[] arcs;
    }

    FORBID_COPY(rdfa_t);
};


static void backprop(const rdfa_t &rdfa, bool *live,
    size_t rule, size_t state)
{
    // "none-rule" is unreachable from final states:
    // be careful to mask it before propagating
    const rdfa_t::state_t &s = rdfa.states[state];
    if (rule == rdfa.nrules) {
        rule = s.rule;
    }

    // if the rule has already been set, than either it's a loop
    // or another branch of back propagation has already been here,
    // in both cases we should stop: there's nothing new to propagate
    bool &l = live[rule * rdfa.nstates + state];
    if (l) return;
    l = true;

    for (const rdfa_t::arc_t *a = s.arcs; a; a = a->next) {
        backprop(rdfa, live, rule, a->dest);
    }
}


static void liveness_analyses(const rdfa_t &rdfa, bool *live)
{
    for (size_t i = 0; i < rdfa.nstates; ++i) {
        const rdfa_t::state_t &s = rdfa.states[i];
        if (s.fallthru) {
            backprop(rdfa, live, s.rule, i);
        }
    }
}


static void warn_dead_rules(const dfa_t &dfa, size_t defrule,
    const std::string &cond, const bool *live, Warn &warn)
{
    const size_t nstates = dfa.states.size();
    const size_t nrules = dfa.rules.size();

    for (size_t i = 0; i < nstates; ++i) {
        const size_t r = dfa.states[i]->rule;
        if (r != Rule::NONE && !live[r * nstates + i]) {
            // skip last rule (it's the NONE-rule)
            for (size_t j = 0; j < nrules; ++j) {
                if (live[j * nstates + i]) {
                    dfa.rules[r].shadow.insert(dfa.rules[j].code->fline);
                }
            }
        }
    }

    for (size_t i = 0; i < nrules; ++i) {
        // default rule '*' should not be reported
        if (i != defrule && !live[i * nstates]) {
            warn.unreachable_rule(cond, dfa.rules[i]);
        }
    }
}


static void remove_dead_final_states(dfa_t &dfa, const bool *fallthru)
{
    const size_t
        nstates = dfa.states.size(),
        nsym = dfa.nchars;

    for (size_t i = 0; i < nstates; ++i) {
        dfa_state_t *s = dfa.states[i];
        if (s->rule == Rule::NONE) continue;

        // final state is useful iff there is at least one
        // non-accepting path from this state
        bool shadowed = true;
        for (size_t c = 0; c < nsym; ++c) {
            const size_t j = s->arcs[c];
            if (j == dfa_t::NIL || fallthru[j]) {
                shadowed = false;
                break;
            }
        }

        if (shadowed) {
            s->rule = Rule::NONE;
            s->tcmd[nsym] = NULL;
        }
    }
}


static void find_fallback_states(dfa_t &dfa, const bool *fallthru)
{
    const size_t
        nstate = dfa.states.size(),
        nsym = dfa.nchars;

    for (size_t i = 0; i < nstate; ++i) {
        dfa_state_t *s = dfa.states[i];

        s->fallthru = fallthru[i];

        if (s->rule != Rule::NONE) {
            for (size_t c = 0; c < nsym; ++c) {
                const size_t j = s->arcs[c];
                if (j != dfa_t::NIL && fallthru[j]) {
                    s->fallback = true;
                    break;
                }
            }
        }
    }
}


void cutoff_dead_rules(dfa_t &dfa, size_t defrule, const std::string &cond, Warn &warn)
{
    const rdfa_t rdfa(dfa);
    const size_t
        ns = rdfa.nstates,
        nl = (rdfa.nrules + 1) * ns;
    bool *live = new bool[nl],
        *fallthru = live + nl - ns;
    memset(live, 0, nl * sizeof(bool));

    liveness_analyses(rdfa, live);
    warn_dead_rules(dfa, defrule, cond, live, warn);
    remove_dead_final_states(dfa, fallthru);
    find_fallback_states(dfa, fallthru);

    delete[] live;
}

} // namespace re2c

