#include "catch.hpp"
#include "doc/docundostack.hpp"
#include <iostream>
#include <memory>
#include <random>

#include <mlt++/MltFactory.h>
#include <mlt++/MltProducer.h>
#include <mlt++/MltProfile.h>
#include <mlt++/MltRepository.h>
#define private public
#define protected public
#include "bin/model/markerlistmodel.hpp"
#include "timeline2/model/clipmodel.hpp"
#include "timeline2/model/compositionmodel.hpp"
#include "timeline2/model/timelineitemmodel.hpp"
#include "timeline2/model/timelinemodel.hpp"
#include "timeline2/model/trackmodel.hpp"
#include "transitions/transitionsrepository.hpp"

QString aCompo;
TEST_CASE("Basic creation/deletion of a composition", "[CompositionModel]")
{
    // Check whether repo works
    QVector<QPair<QString, QString>> transitions = TransitionsRepository::get()->getNames();
    REQUIRE(!transitions.isEmpty());

    // Look for a compo
    for (const auto &trans : transitions) {
        if (TransitionsRepository::get()->isComposition(trans.first)) {
            aCompo = trans.first;
            break;
        }
    }

    REQUIRE(!aCompo.isEmpty());

    // Check construction from repo
    std::unique_ptr<Mlt::Transition> mlt_transition(TransitionsRepository::get()->getTransition(aCompo));

    REQUIRE(mlt_transition->is_valid());

    QByteArray xml;
    std::shared_ptr<DocUndoStack> undoStack = std::make_shared<DocUndoStack>(nullptr);
    std::shared_ptr<MarkerListModel> guideModel(new MarkerListModel(undoStack));
    std::shared_ptr<TimelineItemModel> timeline = TimelineItemModel::construct(new Mlt::Profile(), guideModel, undoStack, xml);

    REQUIRE(timeline->getCompositionsCount() == 0);
    int id1 = CompositionModel::construct(timeline, aCompo);
    REQUIRE(timeline->getCompositionsCount() == 1);

    int id2 = CompositionModel::construct(timeline, aCompo);
    REQUIRE(timeline->getCompositionsCount() == 2);

    int id3 = CompositionModel::construct(timeline, aCompo);
    REQUIRE(timeline->getCompositionsCount() == 3);

    // Test deletion
    REQUIRE(timeline->requestItemDeletion(id2));
    REQUIRE(timeline->getCompositionsCount() == 2);
    REQUIRE(timeline->requestItemDeletion(id3));
    REQUIRE(timeline->getCompositionsCount() == 1);
    REQUIRE(timeline->requestItemDeletion(id1));
    REQUIRE(timeline->getCompositionsCount() == 0);
}

TEST_CASE("Composition manipulation", "[CompositionModel]")
{
    QByteArray xml;
    std::shared_ptr<DocUndoStack> undoStack = std::make_shared<DocUndoStack>(nullptr);
    std::shared_ptr<MarkerListModel> guideModel(new MarkerListModel(undoStack));
    std::shared_ptr<TimelineItemModel> timeline = TimelineItemModel::construct(new Mlt::Profile(), guideModel, undoStack, xml);

    int tid0 = TrackModel::construct(timeline);
    int tid1 = TrackModel::construct(timeline);
    int cid2 = CompositionModel::construct(timeline, aCompo);
    int tid2 = TrackModel::construct(timeline);
    int tid3 = TrackModel::construct(timeline);
    int cid1 = CompositionModel::construct(timeline, aCompo);

    REQUIRE(timeline->getCompositionPlaytime(cid1) == 1);
    REQUIRE(timeline->getCompositionPlaytime(cid2) == 1);

    SECTION("Insert a composition in a track and change track")
    {
        REQUIRE(timeline->checkConsistency());
        REQUIRE(timeline->getTrackCompositionsCount(tid1) == 0);
        REQUIRE(timeline->getTrackCompositionsCount(tid2) == 0);

        REQUIRE(timeline->getCompositionPlaytime(cid1) == 1);
        REQUIRE(timeline->getCompositionPlaytime(cid2) == 1);
        REQUIRE(timeline->getCompositionTrackId(cid1) == -1);
        REQUIRE(timeline->getCompositionPosition(cid1) == -1);

        int pos = 10;
        REQUIRE(timeline->requestCompositionMove(cid1, tid1, pos));
        REQUIRE(timeline->checkConsistency());
        REQUIRE(timeline->getCompositionTrackId(cid1) == tid1);
        REQUIRE(timeline->getCompositionPosition(cid1) == pos);
        REQUIRE(timeline->getCompositionPlaytime(cid1) == 1);
        REQUIRE(timeline->getTrackCompositionsCount(tid1) == 1);
        REQUIRE(timeline->getTrackCompositionsCount(tid2) == 0);

        pos = 1;
        REQUIRE(timeline->requestCompositionMove(cid1, tid2, pos));
        REQUIRE(timeline->checkConsistency());
        REQUIRE(timeline->getCompositionTrackId(cid1) == tid2);
        REQUIRE(timeline->getCompositionPosition(cid1) == pos);
        REQUIRE(timeline->getCompositionPlaytime(cid1) == 1);
        REQUIRE(timeline->getTrackCompositionsCount(tid2) == 1);
        REQUIRE(timeline->getTrackCompositionsCount(tid1) == 0);

        REQUIRE(timeline->requestItemResize(cid1, 10, true));
        REQUIRE(timeline->checkConsistency());
        REQUIRE(timeline->getCompositionTrackId(cid1) == tid2);
        REQUIRE(timeline->getCompositionPosition(cid1) == pos);
        REQUIRE(timeline->getCompositionPlaytime(cid1) == 10);
        REQUIRE(timeline->getTrackCompositionsCount(tid2) == 1);
        REQUIRE(timeline->getTrackCompositionsCount(tid1) == 0);

        REQUIRE(timeline->requestItemResize(cid2, 10, true));
        REQUIRE(timeline->checkConsistency());
        REQUIRE(timeline->getCompositionPlaytime(cid2) == 10);

        // Check conflicts
        int pos2 = timeline->getCompositionPlaytime(cid1);
        REQUIRE(timeline->requestCompositionMove(cid2, tid1, pos2));
        REQUIRE(timeline->checkConsistency());
        REQUIRE(timeline->getCompositionTrackId(cid2) == tid1);
        REQUIRE(timeline->getCompositionPosition(cid2) == pos2);
        REQUIRE(timeline->getTrackCompositionsCount(tid2) == 1);
        REQUIRE(timeline->getTrackCompositionsCount(tid1) == 1);

        REQUIRE_FALSE(timeline->requestCompositionMove(cid1, tid1, pos2 + 2));
        REQUIRE(timeline->checkConsistency());
        REQUIRE(timeline->getTrackCompositionsCount(tid2) == 1);
        REQUIRE(timeline->getTrackCompositionsCount(tid1) == 1);
        REQUIRE(timeline->getCompositionTrackId(cid1) == tid2);
        REQUIRE(timeline->getCompositionPosition(cid1) == pos);
        REQUIRE(timeline->getCompositionTrackId(cid2) == tid1);
        REQUIRE(timeline->getCompositionPosition(cid2) == pos2);

        REQUIRE_FALSE(timeline->requestCompositionMove(cid1, tid1, pos2 - 2));
        REQUIRE(timeline->checkConsistency());
        REQUIRE(timeline->getTrackCompositionsCount(tid2) == 1);
        REQUIRE(timeline->getTrackCompositionsCount(tid1) == 1);
        REQUIRE(timeline->getCompositionTrackId(cid1) == tid2);
        REQUIRE(timeline->getCompositionPosition(cid1) == pos);
        REQUIRE(timeline->getCompositionTrackId(cid2) == tid1);
        REQUIRE(timeline->getCompositionPosition(cid2) == pos2);

        REQUIRE(timeline->requestCompositionMove(cid1, tid1, 0));
        REQUIRE(timeline->checkConsistency());
        REQUIRE(timeline->getTrackCompositionsCount(tid2) == 0);
        REQUIRE(timeline->getTrackCompositionsCount(tid1) == 2);
        REQUIRE(timeline->getCompositionTrackId(cid1) == tid1);
        REQUIRE(timeline->getCompositionPosition(cid1) == 0);
        REQUIRE(timeline->getCompositionTrackId(cid2) == tid1);
        REQUIRE(timeline->getCompositionPosition(cid2) == pos2);
    }

    SECTION("Insert consecutive compositions")
    {
        int length = 12;
        REQUIRE(timeline->requestItemResize(cid1, length, true));
        REQUIRE(timeline->requestItemResize(cid2, length, true));
        REQUIRE(timeline->getCompositionPlaytime(cid1) == length);
        REQUIRE(timeline->getCompositionPlaytime(cid2) == length);
        REQUIRE(timeline->checkConsistency());
        REQUIRE(timeline->getTrackCompositionsCount(tid1) == 0);
        REQUIRE(timeline->getTrackCompositionsCount(tid2) == 0);

        REQUIRE(timeline->requestCompositionMove(cid1, tid1, 0));
        REQUIRE(timeline->checkConsistency());
        REQUIRE(timeline->getCompositionTrackId(cid1) == tid1);
        REQUIRE(timeline->getCompositionPosition(cid1) == 0);
        REQUIRE(timeline->getTrackCompositionsCount(tid1) == 1);

        REQUIRE(timeline->requestCompositionMove(cid2, tid1, length));
        REQUIRE(timeline->checkConsistency());
        REQUIRE(timeline->getCompositionTrackId(cid2) == tid1);
        REQUIRE(timeline->getCompositionPosition(cid2) == length);
        REQUIRE(timeline->getTrackCompositionsCount(tid1) == 2);
    }

    SECTION("Resize orphan composition")
    {
        int length = 12;
        REQUIRE(timeline->requestItemResize(cid1, length, true));
        REQUIRE(timeline->requestItemResize(cid2, length, true));
        REQUIRE(timeline->getCompositionPlaytime(cid1) == length);
        REQUIRE(timeline->getCompositionPlaytime(cid2) == length);

        REQUIRE(timeline->getCompositionPlaytime(cid2) == length);
        REQUIRE(timeline->requestItemResize(cid2, 5, true));
        auto inOut = std::pair<int, int>{0, 4};
        REQUIRE(timeline->m_allCompositions[cid2]->getInOut() == inOut);
        REQUIRE(timeline->getCompositionPlaytime(cid2) == 5);
        REQUIRE(timeline->requestItemResize(cid2, 10, false));
        REQUIRE(timeline->getCompositionPlaytime(cid2) == 10);
        REQUIRE(timeline->requestItemResize(cid2, length + 1, true));
        REQUIRE(timeline->getCompositionPlaytime(cid2) == length + 1);
        REQUIRE(timeline->requestItemResize(cid2, 2, false));
        REQUIRE(timeline->getCompositionPlaytime(cid2) == 2);
        REQUIRE(timeline->requestItemResize(cid2, length, true));
        REQUIRE(timeline->getCompositionPlaytime(cid2) == length);
        REQUIRE(timeline->requestItemResize(cid2, length - 2, true));
        REQUIRE(timeline->getCompositionPlaytime(cid2) == length - 2);
        REQUIRE(timeline->requestItemResize(cid2, length - 3, true));
        REQUIRE(timeline->getCompositionPlaytime(cid2) == length - 3);
    }

    SECTION("Resize inserted compositions")
    {
        int length = 12;
        REQUIRE(timeline->requestItemResize(cid1, length, true));
        REQUIRE(timeline->requestItemResize(cid2, length, true));

        REQUIRE(timeline->requestCompositionMove(cid1, tid1, 0));
        REQUIRE(timeline->checkConsistency());

        REQUIRE(timeline->requestItemResize(cid1, 5, true));
        REQUIRE(timeline->checkConsistency());
        REQUIRE(timeline->getCompositionPlaytime(cid1) == 5);
        REQUIRE(timeline->getCompositionPosition(cid1) == 0);

        REQUIRE(timeline->requestCompositionMove(cid2, tid1, 5));
        REQUIRE(timeline->checkConsistency());
        REQUIRE(timeline->getCompositionPlaytime(cid2) == length);
        REQUIRE(timeline->getCompositionPosition(cid2) == 5);

        REQUIRE_FALSE(timeline->requestItemResize(cid1, 6, true));
        REQUIRE_FALSE(timeline->requestItemResize(cid1, 6, false));
        REQUIRE(timeline->checkConsistency());

        REQUIRE(timeline->requestItemResize(cid2, length - 5, false));
        REQUIRE(timeline->checkConsistency());
        REQUIRE(timeline->getCompositionPosition(cid2) == 10);

        REQUIRE(timeline->requestItemResize(cid1, 10, true));
        REQUIRE(timeline->checkConsistency());
        REQUIRE(timeline->getTrackCompositionsCount(tid1) == 2);
    }

    SECTION("Change track of resized compositions")
    {
        int length = 12;
        REQUIRE(timeline->requestItemResize(cid1, length, true));
        REQUIRE(timeline->requestItemResize(cid2, length, true));

        REQUIRE(timeline->requestCompositionMove(cid2, tid1, 5));
        REQUIRE(timeline->checkConsistency());
        REQUIRE(timeline->getTrackCompositionsCount(tid1) == 1);

        REQUIRE(timeline->requestCompositionMove(cid1, tid2, 10));
        REQUIRE(timeline->checkConsistency());
        REQUIRE(timeline->getTrackCompositionsCount(tid2) == 1);

        REQUIRE(timeline->requestItemResize(cid1, 5, false));
        REQUIRE(timeline->checkConsistency());

        REQUIRE(timeline->requestCompositionMove(cid1, tid1, 0));
        REQUIRE(timeline->checkConsistency());
        REQUIRE(timeline->getTrackCompositionsCount(tid1) == 2);
        REQUIRE(timeline->getTrackCompositionsCount(tid2) == 0);
    }

    SECTION("Composition Move")
    {
        int length = 12;
        REQUIRE(timeline->requestItemResize(cid1, length, true));
        REQUIRE(timeline->requestItemResize(cid2, length, true));

        REQUIRE(timeline->requestCompositionMove(cid2, tid1, 5));
        REQUIRE(timeline->checkConsistency());
        REQUIRE(timeline->getTrackCompositionsCount(tid1) == 1);
        REQUIRE(timeline->getCompositionTrackId(cid2) == tid1);
        REQUIRE(timeline->getCompositionPosition(cid2) == 5);

        REQUIRE(timeline->requestCompositionMove(cid1, tid1, 5 + length));
        auto state = [&]() {
            REQUIRE(timeline->checkConsistency());
            REQUIRE(timeline->getTrackCompositionsCount(tid1) == 2);
            REQUIRE(timeline->getCompositionTrackId(cid1) == tid1);
            REQUIRE(timeline->getCompositionTrackId(cid2) == tid1);
            REQUIRE(timeline->getCompositionPosition(cid1) == 5 + length);
            REQUIRE(timeline->getCompositionPosition(cid2) == 5);
        };
        state();

        REQUIRE_FALSE(timeline->requestCompositionMove(cid1, tid1, 3 + length));
        state();

        REQUIRE_FALSE(timeline->requestCompositionMove(cid1, tid1, 0));
        state();

        REQUIRE(timeline->requestCompositionMove(cid2, tid1, 0));
        auto state2 = [&]() {
            REQUIRE(timeline->checkConsistency());
            REQUIRE(timeline->getTrackCompositionsCount(tid1) == 2);
            REQUIRE(timeline->getCompositionTrackId(cid1) == tid1);
            REQUIRE(timeline->getCompositionTrackId(cid2) == tid1);
            REQUIRE(timeline->getCompositionPosition(cid1) == 5 + length);
            REQUIRE(timeline->getCompositionPosition(cid2) == 0);
        };
        state2();

        REQUIRE_FALSE(timeline->requestCompositionMove(cid1, tid1, 0));
        state2();

        REQUIRE_FALSE(timeline->requestCompositionMove(cid1, tid1, length - 5));
        state2();

        REQUIRE(timeline->requestCompositionMove(cid1, tid1, length));
        REQUIRE(timeline->checkConsistency());
        REQUIRE(timeline->getTrackCompositionsCount(tid1) == 2);
        REQUIRE(timeline->getCompositionTrackId(cid1) == tid1);
        REQUIRE(timeline->getCompositionTrackId(cid2) == tid1);
        REQUIRE(timeline->getCompositionPosition(cid1) == length);
        REQUIRE(timeline->getCompositionPosition(cid2) == 0);

        REQUIRE(timeline->requestItemResize(cid2, length - 5, true));
        REQUIRE(timeline->checkConsistency());
        REQUIRE(timeline->getCompositionTrackId(cid1) == tid1);
        REQUIRE(timeline->getCompositionTrackId(cid2) == tid1);
        REQUIRE(timeline->getCompositionPosition(cid1) == length);
        REQUIRE(timeline->getCompositionPosition(cid2) == 0);

        // REQUIRE(timeline->allowCompositionMove(cid1, tid1, length - 5));
        REQUIRE(timeline->requestCompositionMove(cid1, tid1, length - 5));
        REQUIRE(timeline->checkConsistency());
        REQUIRE(timeline->getTrackCompositionsCount(tid1) == 2);
        REQUIRE(timeline->getCompositionTrackId(cid1) == tid1);
        REQUIRE(timeline->getCompositionTrackId(cid2) == tid1);
        REQUIRE(timeline->getCompositionPosition(cid1) == length - 5);
        REQUIRE(timeline->getCompositionPosition(cid2) == 0);

        REQUIRE(timeline->requestItemResize(cid2, length - 10, false));
        REQUIRE(timeline->checkConsistency());
        REQUIRE(timeline->getCompositionTrackId(cid1) == tid1);
        REQUIRE(timeline->getCompositionTrackId(cid2) == tid1);
        REQUIRE(timeline->getCompositionPosition(cid1) == length - 5);
        REQUIRE(timeline->getCompositionPosition(cid2) == 5);

        REQUIRE_FALSE(timeline->requestCompositionMove(cid1, tid1, 0));
        REQUIRE(timeline->checkConsistency());
        REQUIRE(timeline->getCompositionTrackId(cid1) == tid1);
        REQUIRE(timeline->getCompositionTrackId(cid2) == tid1);
        REQUIRE(timeline->getCompositionPosition(cid1) == length - 5);
        REQUIRE(timeline->getCompositionPosition(cid2) == 5);

        REQUIRE(timeline->requestCompositionMove(cid2, tid1, 0));
        REQUIRE(timeline->checkConsistency());
        REQUIRE(timeline->getTrackCompositionsCount(tid1) == 2);
        REQUIRE(timeline->getCompositionTrackId(cid1) == tid1);
        REQUIRE(timeline->getCompositionTrackId(cid2) == tid1);
        REQUIRE(timeline->getCompositionPosition(cid1) == length - 5);
        REQUIRE(timeline->getCompositionPosition(cid2) == 0);
    }

    SECTION("Move and resize")
    {
        int length = 12;
        REQUIRE(timeline->requestItemResize(cid1, length, true));
        REQUIRE(timeline->requestItemResize(cid2, length, true));

        REQUIRE(timeline->requestCompositionMove(cid1, tid1, 0));
        REQUIRE(timeline->requestItemResize(cid1, length - 2, false));
        REQUIRE(timeline->requestCompositionMove(cid1, tid1, 0));
        auto state = [&]() {
            REQUIRE(timeline->checkConsistency());
            REQUIRE(timeline->getCompositionTrackId(cid1) == tid1);
            REQUIRE(timeline->getTrackCompositionsCount(tid1) == 1);
            REQUIRE(timeline->getCompositionPosition(cid1) == 0);
            REQUIRE(timeline->getCompositionPlaytime(cid1) == length - 2);
        };
        state();

        REQUIRE(timeline->requestItemResize(cid1, length - 4, true));
        REQUIRE(timeline->requestCompositionMove(cid2, tid1, length - 4 + 1));
        REQUIRE(timeline->requestItemResize(cid2, length - 2, false));
        REQUIRE(timeline->requestCompositionMove(cid2, tid1, length - 4 + 1));
        auto state2 = [&]() {
            REQUIRE(timeline->checkConsistency());
            REQUIRE(timeline->getCompositionTrackId(cid1) == tid1);
            REQUIRE(timeline->getCompositionTrackId(cid2) == tid1);
            REQUIRE(timeline->getTrackCompositionsCount(tid1) == 2);
            REQUIRE(timeline->getCompositionPosition(cid1) == 0);
            REQUIRE(timeline->getCompositionPlaytime(cid1) == length - 4);
            REQUIRE(timeline->getCompositionPosition(cid2) == length - 4 + 1);
            REQUIRE(timeline->getCompositionPlaytime(cid2) == length - 2);
        };
        state2();

        // the gap between the two clips is 1 frame, we try to resize them by 2 frames
        REQUIRE_FALSE(timeline->requestItemResize(cid1, length - 2, true));
        state2();
        REQUIRE_FALSE(timeline->requestItemResize(cid2, length, false));
        state2();

        REQUIRE(timeline->requestCompositionMove(cid2, tid1, length - 4));
        auto state3 = [&]() {
            REQUIRE(timeline->checkConsistency());
            REQUIRE(timeline->getCompositionTrackId(cid1) == tid1);
            REQUIRE(timeline->getCompositionTrackId(cid2) == tid1);
            REQUIRE(timeline->getTrackCompositionsCount(tid1) == 2);
            REQUIRE(timeline->getCompositionPosition(cid1) == 0);
            REQUIRE(timeline->getCompositionPlaytime(cid1) == length - 4);
            REQUIRE(timeline->getCompositionPosition(cid2) == length - 4);
            REQUIRE(timeline->getCompositionPlaytime(cid2) == length - 2);
        };
        state3();

        // Now the gap is 0 frames, the resize should still fail
        REQUIRE_FALSE(timeline->requestItemResize(cid1, length - 2, true));
        state3();
        REQUIRE_FALSE(timeline->requestItemResize(cid2, length, false));
        state3();

        // We move cid1 out of the way
        REQUIRE(timeline->requestCompositionMove(cid1, tid2, 0));
        // now resize should work
        REQUIRE(timeline->requestItemResize(cid1, length - 2, true));
        REQUIRE(timeline->requestItemResize(cid2, length, false));
    }
}
